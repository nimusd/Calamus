#ifndef SOUNITBUILDER_H
#define SOUNITBUILDER_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "container.h"
#include "canvas.h"
#include "audioengine.h"
#include "phrase.h"

namespace Ui {
class SounitBuilderCanvas;
}

class SounitBuilder : public QMainWindow
{
    Q_OBJECT




public:
    explicit SounitBuilder(AudioEngine *sharedAudioEngine, QWidget *parent = nullptr);
    ~SounitBuilder();

    // Set the ScoreCanvas to read notes from
    void setScoreCanvas(class ScoreCanvas *canvas);

    // Get the canvas for wiring up inspectors
    Canvas* getCanvas() const { return canvas; }

    // Reconnect container signals (used after loading)
    void connectContainerSignals(Container *container);

protected:
    void keyPressEvent(QKeyEvent *event) override;


private:
    Ui::SounitBuilderCanvas *ui;
    // transport labels
    QLabel *labelPosition ;
    QLabel *labelTempo ;
    QLabel *labelTimeSignature ;
    Container *connectionStartContainer = nullptr;
    QString connectionStartPort;
    bool connectionStartIsOutput = false;
    QVector<Container*> containers;

    Canvas *canvas;

    // Audio engine (shared)
    AudioEngine *audioEngine;

    // Playback state (same as ScoreCanvas)
    QTimer *playbackTimer;
    int currentNoteIndex;
    double playbackStartTime;
    double noteDuration;  // Total duration of rendered buffer
    bool isPlaying;

    // ScoreCanvas reference to get notes from (shared with ScoreCanvasWindow)
    class ScoreCanvas *scoreCanvas;

private slots:
    void onAddContainer(const QString &name, const QColor &color,
                        const QStringList &inputs, const QStringList &outputs);

    void onPortClicked(Container *container, const QString &portName, bool isOutput, QPoint globalPos);

    void onPlay();
    void onStop();
    void onPlaybackTick();

public slots:
    void stopPlayback(bool stopAudioEngine = true);
    void rebuildGraph();  // Rebuild audio graph from canvas

private:
    void startPlayback();

signals:
    void playbackStarted();






};

#endif // SOUNITBUILDER_H
