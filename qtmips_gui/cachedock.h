#ifndef CACHEDOCK_H
#define CACHEDOCK_H

#include <QDockWidget>
#include <QLabel>
#include <QFormLayout>
#include "cacheview.h"
#include "graphicsview.h"
#include "qtmipsmachine.h"

class CacheDock : public QDockWidget {
    Q_OBJECT
public:
    CacheDock(QWidget *parent, const QString &type);

    void setup(const machine::Cache *cache);

private slots:
    void hit_update(unsigned);
    void miss_update(unsigned);

private:
    QVBoxLayout *layout_box;
    QWidget *top_widget, *top_form;
    QFormLayout *layout_top_form;
    QLabel *l_hit, *l_miss, *no_cache;
    GraphicsView *graphicsview;
    CacheViewScene *cachescene;
};

#endif // CACHEDOCK_H