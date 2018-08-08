#include "qss.h"

const QString LeftButtonEnabled = "QPushButton { background-color: lightgray }";
// 在#后跟objectName, 指明具体的button
const QString LeftButtonPressed = "QPushButton#%1 { "
                                  "background-color: gray;"
                                  "border-width: 2px; "
                                  "border-style: solid; "
                                  "border-color: white; "
                                  "}";
