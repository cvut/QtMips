#include "memorydock.h"

MemoryDock::MemoryDock(QWidget *parent) : QDockWidget(parent) {
    memory_view = new MemoryView(this);
    setWidget(memory_view);

    setObjectName("Memory");
    setWindowTitle("Memory");
}

MemoryDock::~MemoryDock() {
    delete memory_view;
}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    if (machine == nullptr)
        // TODO reset memory view
        return;

    // TODO setup memory view

}