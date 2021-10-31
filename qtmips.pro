TEMPLATE = subdirs

SUBDIRS += \
    libelf \
    qtmips_machine \
    qtmips_osemu \
    qtmips_asm

!wasm: SUBDIRS += \
    qtmips_machine/tests \
    qtmips_cli \

SUBDIRS += \
    qtmips_gui

qtmips_cli.depends = qtmips_machine qtmips_asm
qtmips_gui.depends = qtmips_machine qtmips_osemu qtmips_asm
qtmips_machine-tests.depends = qtmips_machine
