#ifndef COREVIEW_H
#define COREVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "qtmipsmachine.h"
#include "coreview/connection.h"
#include "coreview/programcounter.h"
#include "coreview/multiplexer.h"
#include "coreview/latch.h"
#include "coreview/alu.h"
#include "coreview/adder.h"
#include "coreview/memory.h"
#include "coreview/registers.h"
#include "coreview/instructionview.h"
#include "coreview/junction.h"
#include "coreview/constant.h"

class CoreView : public QGraphicsView {
public:
    CoreView(QWidget *parent);

    void setScene(QGraphicsScene *scene);

protected:
    void resizeEvent(QResizeEvent *event);
private:
    void update_scale();
};

class CoreViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CoreViewScene(CoreView *view, machine::QtMipsMachine *machine);
    ~CoreViewScene();

signals:
    void request_registers();
    void request_data_memory();
    void request_program_memory();

private:
    coreview::ProgramCounter *pc;
    coreview::Latch *pc_latch;
    coreview::Adder *pc_adder;
    coreview::Constant *pc_adder_4;
    coreview::Junction *pc_junction;
    coreview::Alu *alu;
    coreview::Memory *mem;
    coreview::Registers *regs;
    coreview::Multiplexer *pc_multiplexer, *mem_or_reg;

    coreview::Connection *pc2pc_latch;
    coreview::Connection *pc_latch2pc_joint, *pc_joint2pc_adder, *pc_joint2mem;
    coreview::Connection *pc_multiplexer2pc;
};

class CoreViewSceneSimple : public CoreViewScene {
public:
    CoreViewSceneSimple(CoreView *view, machine::QtMipsMachine *machine);
    ~CoreViewSceneSimple();

private:
    coreview::Latch *delay_slot_latch;
};

class CoreViewScenePipelined : public CoreViewScene {
public:
    CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine);
    ~CoreViewScenePipelined();

private:
    coreview::Latch *latch_if_id, *latch_id_ex, *latch_ex_mem, *latch_mem_wb;
    // TODO forwarding unit
};

#else

class CoreView;
class CoreViewScene;
class CoreViewSceneSimple;
class CoreViewScenePipelined;

#endif // COREVIEW_H
