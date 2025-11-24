#include "ventanaprincipal.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent)
{
    m_escena = new EscenaJuego(this);
    m_vista = new QGraphicsView(m_escena);
    m_vista->setRenderHint(QPainter::Antialiasing);
    m_vista->setMinimumSize(900, 500);

    auto *central = new QWidget;
    auto *layoutVertical = new QVBoxLayout(central);
    layoutVertical->addWidget(m_vista);

    auto *layoutControles = new QHBoxLayout;

    m_spinAngulo = new QDoubleSpinBox;
    m_spinAngulo->setRange(5, 85);
    m_spinAngulo->setValue(45);

    m_spinVelocidad = new QDoubleSpinBox;
    m_spinVelocidad->setRange(50, 300);
    m_spinVelocidad->setValue(150);

    m_botonDisparar = new QPushButton(tr("Disparar"));
    m_etiquetaTurno = new QLabel(tr("Turno: jugador izquierda"));

    layoutControles->addWidget(new QLabel(tr("Ángulo (°):")));
    layoutControles->addWidget(m_spinAngulo);
    layoutControles->addWidget(new QLabel(tr("Velocidad:")));
    layoutControles->addWidget(m_spinVelocidad);
    layoutControles->addWidget(m_botonDisparar);
    layoutControles->addWidget(m_etiquetaTurno);

    layoutVertical->addLayout(layoutControles);

    setCentralWidget(central);
    setWindowTitle(tr("Práctica 5 - Juego de artillería"));

    connect(m_botonDisparar, &QPushButton::clicked,
            this, &VentanaPrincipal::botonDisparar);
    connect(m_escena, &EscenaJuego::turnoCambiado,
            this, &VentanaPrincipal::actualizarEtiquetaTurno);
    connect(m_escena, &EscenaJuego::partidaTerminada,
            this, &VentanaPrincipal::mostrarGanador);
}

void VentanaPrincipal::botonDisparar()
{
    m_escena->dispararProyectil(
        m_spinAngulo->value(),
        m_spinVelocidad->value());
}

void VentanaPrincipal::actualizarEtiquetaTurno(EscenaJuego::Bando bando)
{
    if (bando == EscenaJuego::Izquierda)
        m_etiquetaTurno->setText(tr("Turno: jugador izquierda"));
    else
        m_etiquetaTurno->setText(tr("Turno: jugador derecha"));
}

void VentanaPrincipal::mostrarGanador(EscenaJuego::Bando ganador)
{
    QString msg = (ganador == EscenaJuego::Izquierda)
    ? tr("¡Gana el jugador de la izquierda!")
    : tr("¡Gana el jugador de la derecha!");
    QMessageBox::information(this, tr("Fin de la partida"), msg);
}
