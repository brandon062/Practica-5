#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include "escenajuego.h"

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT
public:
    explicit VentanaPrincipal(QWidget *parent = nullptr);

private slots:
    void botonDisparar();
    void actualizarEtiquetaTurno(EscenaJuego::Bando bando);
    void mostrarGanador(EscenaJuego::Bando ganador);

protected:
    // ---  para capturar la tecla R ---
    void keyPressEvent(QKeyEvent *event) override;
private:
    EscenaJuego    *m_escena;
    QGraphicsView  *m_vista;
    QDoubleSpinBox *m_spinAngulo;
    QDoubleSpinBox *m_spinVelocidad;
    QPushButton    *m_botonDisparar;
    QLabel         *m_etiquetaTurno;
};

#endif // VENTANAPRINCIPAL_H
