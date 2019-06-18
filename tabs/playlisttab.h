#pragma once

#include "fap.h"

void pTabUpdateList(Ui::Fap *ui, Player *mpd);
void pTabUpdateTree(Ui::Fap *ui, Player *mpd, QString name);
void pTabListContextMenu(Ui::Fap *ui, Player *mpd, const QPoint &pos);
void pTabTreeItemDoubleClicked(Player *mpd, QTreeWidgetItem *item);
