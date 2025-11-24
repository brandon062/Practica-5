#include "escenajuego.h"
#include <QtMath>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
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
    double separacion = 20;
    double altoTecho = 60;
    double xBaseIzq = 150;

    // ----------- LADO IZQUIERDO (bloques + rival) -----------
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

    // "Rival" izquierdo detrás de la estructura
    m_rivalIzquierda = addRect(
        xBaseIzq + anchoColumna*0.5, yBase - altoColumna + 20,
        anchoColumna, altoColumna - 40,
        QPen(Qt::black), QBrush(QColor(255,230,200)));

    // ----------- LADO DERECHO (bloques + rival) -----------
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

    m_rivalDerecha = addRect(
        xBaseDer + anchoColumna*0.5, yBase - altoColumna + 20,
        anchoColumna, altoColumna - 40,
        QPen(Qt::black), QBrush(QColor(255,230,200)));

    // ----------- CAÑONES CENTRADOS EN LOS LATERALES -----------

    // Altura media de la escena
    double yCentro = m_alto / 2.0;

    // Plataforma izquierda (no recibe daño, solo decorativa)
    m_plataformaIzquierda = addRect(
        20, yCentro + 10,      // posición
        70, 10,                // ancho/alto
        QPen(Qt::black), QBrush(Qt::darkGray));

    // Cañón izquierdo encima de la plataforma
    m_canionIzquierda = addRect(
        30, yCentro - 10,      // posición
        40, 20,                // ancho/alto
        QPen(Qt::black), QBrush(Qt::blue));

    // Plataforma derecha
    m_plataformaDerecha = addRect(
        m_ancho - 90, yCentro + 10,
        70, 10,
        QPen(Qt::black), QBrush(Qt::darkGray));

    // Cañón derecho encima de la plataforma
    m_canionDerecha = addRect(
        m_ancho - 70, yCentro - 10,
        40, 20,
        QPen(Qt::black), QBrush(Qt::blue));
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
        // Punto de salida: centro del cañón izquierdo
        QRectF r = m_canionIzquierda->rect();
        QPointF centro = r.center() + m_canionIzquierda->pos();
        m_proyectil.posicion = Vector2D(centro.x(), centro.y());
        m_proyectil.velocidad.x =  velocidad * qCos(rad);
        m_proyectil.velocidad.y = -velocidad * qSin(rad);
    } else {
        // Punto de salida: centro del cañón derecho
        QRectF r = m_canionDerecha->rect();
        QPointF centro = r.center() + m_canionDerecha->pos();
        m_proyectil.posicion = Vector2D(centro.x(), centro.y());
        m_proyectil.velocidad.x = -velocidad * qCos(rad);
        m_proyectil.velocidad.y = -velocidad * qSin(rad);
    }

    // Si el ítem gráfico del proyectil no existe, lo creamos.
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

    // Condiciones para "matar" el proyectil y pasar turno.
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
