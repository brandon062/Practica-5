#include "escenajuego.h"
#include <QtMath>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QTransform>
#include <algorithm>    // std::min, std::max
#include <cmath>        // std::abs

EscenaJuego::EscenaJuego(QObject *parent)
    : QGraphicsScene(parent),
    m_turno(Izquierda)
{
    setSceneRect(0, 0, m_ancho, m_alto);
    configurarMundo();

    // Temporizador que avanza la simulación a ~60 FPS
    m_temporizador.setInterval(16);
    connect(&m_temporizador, &QTimer::timeout,
            this, &EscenaJuego::actualizarSimulacion);
}

// Crea todos los elementos de la escena (bloques, rivales, cañones, etc.)
void EscenaJuego::configurarMundo()
{
    // Fondo (cielo) y "suelo" verde
    setBackgroundBrush(QBrush(QColor(220, 230, 255)));
    addRect(0, m_alto-20, m_ancho, 20,
            QPen(Qt::NoPen), QBrush(Qt::darkGreen));

    // Geometría común de las estructuras
    double yBase = m_alto - 20;
    double anchoColumna = 60;
    double altoColumna = 200;
    double separacion = 45;
    double altoTecho = 60;

    // Un poco más adentro de la pared izquierda
    double xBaseIzq = 200;

    // ----------- LADO IZQUIERDO (bloques) -----------
    BloqueEstructura *izqCol1 = new BloqueEstructura(
        QRectF(xBaseIzq, yBase - altoColumna,
               anchoColumna, altoColumna), 200);
    BloqueEstructura *izqCol2 = new BloqueEstructura(
        QRectF(xBaseIzq + anchoColumna + separacion, yBase - altoColumna,
               anchoColumna, altoColumna), 200);
    addItem(izqCol1); addItem(izqCol2);
    m_bloquesIzquierda << izqCol1 << izqCol2;

    double yTecho = yBase - altoColumna - altoTecho;
    BloqueEstructura *izqTecho = new BloqueEstructura(
        QRectF(xBaseIzq, yTecho,
               2*anchoColumna + separacion, altoTecho), 100);
    addItem(izqTecho);
    m_bloquesIzquierda << izqTecho;

    // ----------- LADO DERECHO (bloques) -----------
    double xBaseDer = m_ancho - xBaseIzq - 2*anchoColumna - separacion;

    BloqueEstructura *derCol1 = new BloqueEstructura(
        QRectF(xBaseDer, yBase - altoColumna,
               anchoColumna, altoColumna), 200);
    BloqueEstructura *derCol2 = new BloqueEstructura(
        QRectF(xBaseDer + anchoColumna + separacion, yBase - altoColumna,
               anchoColumna, altoColumna), 200);
    addItem(derCol1); addItem(derCol2);
    m_bloquesDerecha << derCol1 << derCol2;

    BloqueEstructura *derTecho = new BloqueEstructura(
        QRectF(xBaseDer, yTecho,
               2*anchoColumna + separacion, altoTecho), 100);
    addItem(derTecho);
    m_bloquesDerecha << derTecho;

    // ----------- SPRITES DE PERSONAJES -----------

    // Cargamos sprites y se les ajusta el tamaño
    QPixmap spritePersonaje1(":/new/images/personaje1.png");
    spritePersonaje1 = spritePersonaje1.scaled(
        100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap spritePersonaje2(":/new/images/personaje2.png");
    spritePersonaje2 = spritePersonaje2.scaled(
        100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Personaje izquierdo = personaje1.png
    m_rivalIzquierda = addPixmap(spritePersonaje1);
    m_rivalIzquierda->setZValue(1);  // delante de bloques

    // Se coloca centrado ENTRE las dos columnas
    double anchoEstructura = 2*anchoColumna + separacion;
    double xCentroEstrIzq = xBaseIzq + anchoEstructura / 2.0;
    double yPersonaje = yBase - spritePersonaje1.height() - 10;

    m_rivalIzquierda->setPos(xCentroEstrIzq - spritePersonaje1.width()/2.0,
                             yPersonaje);

    // Personaje derecho = personaje2.png, también centrado en su estructura
    m_rivalDerecha = addPixmap(spritePersonaje2);
    m_rivalDerecha->setZValue(1);

    double xCentroEstrDer = xBaseDer + anchoEstructura / 2.0;

    m_rivalDerecha->setPos(xCentroEstrDer - spritePersonaje2.width()/2.0,
                           yPersonaje);

    // ----------- CAÑONES CENTRADOS EN LOS LATERALES (sprites) -----------

    // Sprite base del cañón, escalado
    QPixmap spriteCanon(":/new/images/canon.png");
    spriteCanon = spriteCanon.scaled(
        80, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Versión espejada para el cañón izquierdo
    QPixmap spriteCanonIzq = spriteCanon.transformed(
        QTransform().scale(-1, 1));

    double yCentro = m_alto / 2.0;

    double anchoPlataforma = 80;
    double altoPlataforma  = 10;
    double xPlataformaMargen = 10;   // separación de la pared

    // Plataforma izquierda (no recibe daño)
    m_plataformaIzquierda = addRect(
        xPlataformaMargen,
        yCentro + 10,
        anchoPlataforma,
        altoPlataforma,
        QPen(Qt::black), QBrush(Qt::darkGray));

    // Cañón izquierdo: usa sprite espejado (mira a la derecha)
    m_canionIzquierda = addPixmap(spriteCanonIzq);
    m_canionIzquierda->setZValue(1);

    double xCanonIzq = xPlataformaMargen + anchoPlataforma/2.0;
    double yCanon    = yCentro - spriteCanonIzq.height()/2.0;

    m_canionIzquierda->setPos(xCanonIzq - spriteCanonIzq.width()/2.0,
                              yCanon);

    // Plataforma derecha
    m_plataformaDerecha = addRect(
        m_ancho - xPlataformaMargen - anchoPlataforma,
        yCentro + 10,
        anchoPlataforma,
        altoPlataforma,
        QPen(Qt::black), QBrush(Qt::darkGray));

    // Cañón derecho: sprite original (mira hacia la izquierda)
    m_canionDerecha = addPixmap(spriteCanon);
    m_canionDerecha->setZValue(1);

    double xCanonDer = m_ancho - xPlataformaMargen - anchoPlataforma/2.0;
    m_canionDerecha->setPos(xCanonDer - spriteCanon.width()/2.0,
                            yCanon);
}

// Lógica para iniciar un disparo desde el bando que tenga el turno.
void EscenaJuego::dispararProyectil(double anguloGrados, double velocidad)
{
    if (m_proyectil.activo) return; // aún hay un proyectil en vuelo
    reiniciarProyectil(m_turno, anguloGrados, velocidad);
    m_temporizador.start();
}

// Posiciona y configura el proyectil según el bando (izquierda/derecha).

void EscenaJuego::reiniciarProyectil(Bando bando,
                                     double anguloGrados,
                                     double velocidad)
{
    m_proyectil.activo = true;
    m_proyectil.tiempoVida = 0.0;
    m_proyectil.masa = 10.0;
    m_proyectil.radio = 8.0;

    double rad = qDegreesToRadians(anguloGrados);

    if (bando == Izquierda) {
        // Centro del cañón izquierdo
        QRectF r = m_canionIzquierda->boundingRect();
        QPointF centro = m_canionIzquierda->mapToScene(r.center());
        m_proyectil.posicion = Vector2D(centro.x(), centro.y());

        // Disparo hacia la derecha
        m_proyectil.velocidad.x =  velocidad * qCos(rad);
        m_proyectil.velocidad.y = -velocidad * qSin(rad);
    } else {
        // Centro del cañón derecho
        QRectF r = m_canionDerecha->boundingRect();
        QPointF centro = m_canionDerecha->mapToScene(r.center());
        m_proyectil.posicion = Vector2D(centro.x(), centro.y());

        // Disparo hacia la izquierda
        m_proyectil.velocidad.x = -velocidad * qCos(rad);
        m_proyectil.velocidad.y = -velocidad * qSin(rad);
    }

    if (!m_itemProyectil) {
        m_itemProyectil = addEllipse(
            0, 0,
            2*m_proyectil.radio, 2*m_proyectil.radio,
            QPen(Qt::black), QBrush(Qt::red));
    }
    m_itemProyectil->setVisible(true);
    m_itemProyectil->setPos(m_proyectil.posicion.x - m_proyectil.radio,
                            m_proyectil.posicion.y - m_proyectil.radio);
}

// Avanza la simulación un paso de tiempo.
void EscenaJuego::actualizarSimulacion()
{
    if (!m_proyectil.activo){
        m_temporizador.stop();
        return;
    }

    double dt = 0.016;
    integrar(dt);
    resolverChoquesParedes();
    resolverChoquesBloques();
    comprobarGolpeRival();

    m_proyectil.tiempoVida += dt;
    double vel = magnitud(m_proyectil.velocidad);

    // Condiciones para eliminar el proyectil y pasar turno.
    if (m_proyectil.posicion.y - m_proyectil.radio > m_alto + 50 ||
        m_proyectil.posicion.x + m_proyectil.radio < -50 ||
        m_proyectil.posicion.x - m_proyectil.radio > m_ancho + 50 ||
        m_proyectil.tiempoVida > 8.0 || vel < 10.0)
    {
        finalizarTurno();
    }
}

// Integración explícita muy simple (Euler).
void EscenaJuego::integrar(double dt)
{
    // Aceleración: solo gravedad hacia abajo.
    m_proyectil.velocidad.y += m_gravedad * dt;
    m_proyectil.posicion += m_proyectil.velocidad * dt;

    if (m_itemProyectil)
        m_itemProyectil->setPos(m_proyectil.posicion.x - m_proyectil.radio,
                                m_proyectil.posicion.y - m_proyectil.radio);
}

// Colisiones elásticas con las paredes de la escena (caja).
void EscenaJuego::resolverChoquesParedes()
{
    bool rebote = false;

    if (m_proyectil.posicion.x - m_proyectil.radio < 0.0){
        m_proyectil.posicion.x = m_proyectil.radio;
        m_proyectil.velocidad.x *= -1.0;
        rebote = true;
    }
    if (m_proyectil.posicion.x + m_proyectil.radio > m_ancho){
        m_proyectil.posicion.x = m_ancho - m_proyectil.radio;
        m_proyectil.velocidad.x *= -1.0;
        rebote = true;
    }
    if (m_proyectil.posicion.y - m_proyectil.radio < 0.0){
        m_proyectil.posicion.y = m_proyectil.radio;
        m_proyectil.velocidad.y *= -1.0;
        rebote = true;
    }
    if (m_proyectil.posicion.y + m_proyectil.radio > m_alto - 20){
        m_proyectil.posicion.y = m_alto - 20 - m_proyectil.radio;
        m_proyectil.velocidad.y *= -1.0;
        rebote = true;
    }

    if (rebote && m_itemProyectil)
        m_itemProyectil->setPos(m_proyectil.posicion.x - m_proyectil.radio,
                                m_proyectil.posicion.y - m_proyectil.radio);
}

// Chequeo geométrico: ¿un círculo intersecta un rectángulo?
bool EscenaJuego::circuloIntersecaRect(const Vector2D &c, double r,
                                       const QRectF &rect) const
{
    double qx = std::max(rect.left(),  std::min(rect.right(),  c.x));
    double qy = std::max(rect.top(),   std::min(rect.bottom(), c.y));
    Vector2D d(c.x - qx, c.y - qy);
    return magnitud2(d) <= r*r;
}

// Colisiones inelásticas contra los bloques de la infraestructura.
void EscenaJuego::resolverChoquesBloques()
{
    auto procesarLista = [&](QVector<BloqueEstructura*> &lista){
        for (BloqueEstructura *b : lista){
            if (!b || b->destruido()) continue;

            QRectF r = b->sceneBoundingRect();
            if (!circuloIntersecaRect(m_proyectil.posicion,
                                      m_proyectil.radio, r))
                continue;

            // Aproximamos la normal de choque eligiendo la cara más cercana.
            double distIzq  = std::abs((m_proyectil.posicion.x + m_proyectil.radio) - r.left());
            double distDer  = std::abs((m_proyectil.posicion.x - m_proyectil.radio) - r.right());
            double distSup  = std::abs((m_proyectil.posicion.y + m_proyectil.radio) - r.top());
            double distInf  = std::abs((m_proyectil.posicion.y - m_proyectil.radio) - r.bottom());

            Vector2D n(0,0);
            double minDist = std::min(std::min(distIzq, distDer),
                                      std::min(distSup, distInf));

            if (minDist == distIzq)       n = Vector2D(-1,0);
            else if (minDist == distDer)  n = Vector2D(1,0);
            else if (minDist == distSup)  n = Vector2D(0,-1);
            else                          n = Vector2D(0,1);

            // Separación mínima para evitar que se "clave" en el bloque.
            m_proyectil.posicion += n * 1.0;

            // Descomposición de la velocidad en normal y tangencial.
            double vN = productoPunto(m_proyectil.velocidad, n);
            Vector2D vPerp = n * vN;
            Vector2D vPar  = m_proyectil.velocidad - vPerp;

            // Rebote inelástico (se pierde energía en la normal).
            Vector2D vPerpNueva = n * (-m_coefRestEstructura * vN);
            m_proyectil.velocidad = vPar + vPerpNueva;

            if (m_itemProyectil)
                m_itemProyectil->setPos(m_proyectil.posicion.x - m_proyectil.radio,
                                        m_proyectil.posicion.y - m_proyectil.radio);

            // Daño proporcional al momento (masa * |velocidad|)
            double vel = magnitud(m_proyectil.velocidad);
            double danio = m_factorDanio * m_proyectil.masa * vel;
            b->aplicarDanio(danio);
        }
    };

    procesarLista(m_bloquesIzquierda);
    procesarLista(m_bloquesDerecha);
}

// Comprueba si el proyectil golpea al rival y decide la victoria.
void EscenaJuego::comprobarGolpeRival()
{
    auto todosDestruidos = [](const QVector<BloqueEstructura*> &lista){
        for (auto *b : lista)
            if (b && !b->destruido()) return false;
        return true;
    };

    if (!m_proyectil.activo) return;

    if (m_turno == Izquierda) {
        if (!todosDestruidos(m_bloquesDerecha)) return;

        if (circuloIntersecaRect(m_proyectil.posicion, m_proyectil.radio,
                                 m_rivalDerecha->sceneBoundingRect())){
            m_proyectil.activo = false;
            if (m_itemProyectil) m_itemProyectil->setVisible(false);
            emit partidaTerminada(Izquierda);
            m_temporizador.stop();
        }
    } else {
        if (!todosDestruidos(m_bloquesIzquierda)) return;

        if (circuloIntersecaRect(m_proyectil.posicion, m_proyectil.radio,
                                 m_rivalIzquierda->sceneBoundingRect())){
            m_proyectil.activo = false;
            if (m_itemProyectil) m_itemProyectil->setVisible(false);
            emit partidaTerminada(Derecha);
            m_temporizador.stop();
        }
    }
}

// Destruye el proyectil actual y alterna el turno.
void EscenaJuego::finalizarTurno()
{
    m_proyectil.activo = false;
    if (m_itemProyectil) m_itemProyectil->setVisible(false);
    m_temporizador.stop();

    m_turno = (m_turno == Izquierda) ? Derecha : Izquierda;
    emit turnoCambiado(m_turno);
}
