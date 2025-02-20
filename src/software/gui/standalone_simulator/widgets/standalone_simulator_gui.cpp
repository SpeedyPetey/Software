#include "software/gui/standalone_simulator/widgets/standalone_simulator_gui.h"

#include "proto/message_translation/ssl_geometry.h"
#include "software/gui/drawing/ssl_wrapper_packet.h"
#include "software/gui/geometry_conversion.h"
#include "software/world/field.h"

StandaloneSimulatorGUI::StandaloneSimulatorGUI(
    std::shared_ptr<StandaloneSimulator> simulator,
    std::shared_ptr<SimulatorConfig> mutable_simulator_config,
    std::shared_ptr<StandaloneSimulatorConfig> mutable_standalone_simulator_config)
    : QMainWindow(),
      main_widget(new Ui::AutogeneratedStandaloneSimulatorMainWidget()),
      update_timer(new QTimer(this)),
      standalone_simulator(simulator),
      mutable_simulator_config(mutable_simulator_config),
      mutable_standalone_simulator_config(mutable_standalone_simulator_config),
      remaining_attempts_to_set_view_area(NUM_ATTEMPTS_TO_SET_INITIAL_VIEW_AREA)
{
    if (!simulator)
    {
        throw std::invalid_argument(
            "Cannot create StandaloneSimulatorGUI without a valid StandaloneSimulator");
    }

    // Create a new widget that will contain all the autogenerated
    // components defined in the .ui file. Note that because the
    // setCentralWidget call will cause this class (the Qt QMainWindow)
    // to take ownership of the widget and handle its deletion, we
    // do not need to make this a class member and delete it ourself.
    QWidget *central_widget = new QWidget(this);
    main_widget->setupUi(central_widget);
    setCentralWidget(central_widget);

    connect(main_widget->play_button, &QRadioButton::toggled, [&](bool checked) {
        if (checked)
        {
            standalone_simulator->resetSlowMotionMultiplier();
            standalone_simulator->startSimulation();
        }
    });
    connect(main_widget->pause_button, &QRadioButton::toggled, [&](bool checked) {
        if (checked)
        {
            standalone_simulator->stopSimulation();
        }
    });
    connect(main_widget->slow_motion_button, &QRadioButton::toggled, [&](bool checked) {
        if (checked)
        {
            standalone_simulator->setSlowMotionMultiplier(
                StandaloneSimulator::DEFAULT_SLOW_MOTION_MULTIPLIER);
            standalone_simulator->startSimulation();
        }
    });

    main_widget->simulation_graphics_view->setStandaloneSimulator(standalone_simulator);
    main_widget->simulator_dynamic_parameter_widget->setupParameters(
        mutable_simulator_config);
    main_widget->standalone_simulator_dynamic_parameter_widget->setupParameters(
        mutable_standalone_simulator_config);
    // standalone_simulator_dynamic_parameter_widget will match the height of
    // simulator_dynamic_parameter_widget
    main_widget->standalone_simulator_dynamic_parameter_widget->setSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Ignored);

    connect(update_timer, &QTimer::timeout, this, &StandaloneSimulatorGUI::handleUpdate);
    update_timer->start(static_cast<int>(
        Duration::fromSeconds(UPDATE_INTERVAL_SECONDS).toMilliseconds()));
}

void StandaloneSimulatorGUI::handleUpdate()
{
    draw();
    updateDrawViewArea();
}

void StandaloneSimulatorGUI::draw()
{
    auto ssl_wrapper_packet = standalone_simulator->getSSLWrapperPacket();
    main_widget->simulation_graphics_view->clearAndDraw(
        {main_widget->simulation_graphics_view->getDrawBallVelocityFunction()
             .getDrawFunction(),
         getDrawSSLWrapperPacketFunction(ssl_wrapper_packet,
                                         standalone_simulator->getRobotConstants())
             .getDrawFunction()});
}

void StandaloneSimulatorGUI::updateDrawViewArea()
{
    if (remaining_attempts_to_set_view_area > 0)
    {
        auto ssl_wrapper_packet = standalone_simulator->getSSLWrapperPacket();
        if (ssl_wrapper_packet.has_geometry())
        {
            if (auto field = createField(ssl_wrapper_packet.geometry()))
            {
                Rectangle area = field->fieldBoundary();
                main_widget->simulation_graphics_view->setViewArea(area);
            }
        }
        remaining_attempts_to_set_view_area--;
    }
}
