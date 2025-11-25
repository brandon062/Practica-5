#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsScene>
#include <QTimer>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "vector2d.h"
#include "bloqueestructura.h"
#include <QGraphicsTextItem>

class EscenaJuego : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Bando { Izquierda, Derecha };
    Q_ENUM(Bando)

    explicit EscenaJuego(QObject *parent = nullptr);

    Bando turnoActual() const { return m_turno; }
    void dispararProyectil(double anguloGrados, double velocidad);

    // --- reiniciar el juego ---
    Q_INVOKABLE void reiniciarJuego();

signals:
    void turnoCambiado(EscenaJuego::Bando nuevoTurno);
    void partidaTerminada(EscenaJuego::Bando ganador);

private slots:
    void actualizarSimulacion();

private:
    struct EstadoProyectil {
        bool     activo{false};
        double   masa{10.0};
        double   radio{8.0};
        Vector2D posicion;
        Vector2D velocidad;
        double   tiempoVida{0.0};
    };

    void configurarMundo();
    void reiniciarProyectil(Bando bando, double anguloGrados, double velocidad);
    void integrar(double dt);
    void resolverChoquesParedes();
    void resolverChoquesBloques();
    void comprobarGolpeRival();
    void finalizarTurno();
    bool circuloIntersecaRect(const Vector2D &c, double r, const QRectF &rect) const;

    Bando m_turno;
    EstadoProyectil m_proyectil;
    QGraphicsEllipseItem *m_itemProyectil{nullptr};
    QTimer m_temporizador;

    QVector<BloqueEstructura*> m_bloquesIzquierda;
    QVector<BloqueEstructura*> m_bloquesDerecha;

    QGraphicsPixmapItem *m_rivalIzquierda{nullptr};
    QGraphicsPixmapItem *m_rivalDerecha{nullptr};

    QGraphicsPixmapItem *m_canionIzquierda{nullptr};
    QGraphicsPixmapItem *m_canionDerecha{nullptr};
    QGraphicsRectItem   *m_plataformaIzquierda{nullptr};
    QGraphicsRectItem   *m_plataformaDerecha{nullptr};

    double m_ancho{1200.0};
    double m_alto{600.0};

    double m_gravedad{200.0};
    double m_coefRestEstructura{0.5};
    double m_factorDanio{0.02};

    // --- Sonidos ---
    QMediaPlayer *sonidoDisparo{nullptr};
    QAudioOutput *audioDisparo{nullptr};

    QMediaPlayer *sonidoRebote{nullptr};
    QAudioOutput *audioRebote{nullptr};

    QMediaPlayer *sonidoDestruccion{nullptr};
    QAudioOutput *audioDestruccion{nullptr};

    // --- Música de fondo ---
    QMediaPlayer *musicaFondo1{nullptr};
    QAudioOutput *audioMusica1{nullptr};

    QMediaPlayer *musicaFondo2{nullptr};
    QAudioOutput *audioMusica2{nullptr};

    // --- sonido de victoria ---
    QMediaPlayer *sonidoVictoria{nullptr};
    QAudioOutput *audioVictoria{nullptr};

    // --- texto de fin de partida ---
    QGraphicsTextItem *m_textoFin{nullptr};
    bool m_hayGanador{false};
};

#endif // ESCENAJUEGO_H




