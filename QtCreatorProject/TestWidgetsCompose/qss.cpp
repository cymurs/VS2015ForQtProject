#include "qss.h"

const QString LeftButtonEnabled = "QPushButton { background-color: lightgray }";
// 在#后跟objectName, 指明具体的button
const QString LeftButtonPressed = "QPushButton#%1 { "
                                  "background-color: gray;"
                                  "border: 2px solid #8f8f91; "
                                  "}";
