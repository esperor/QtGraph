project(QtGraph)

set(QTGRAPH_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/nodetypemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/pintypemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/nodefactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/typemanager.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/nodespawndata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/pindata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/pindragsignal.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/abstractpin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/basenode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/nfbuttonminimize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typebrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/canvas.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typednodeimage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/pin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typednode.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/utilities/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/utilities/idgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/utilities/utility.h

    PARENT_SCOPE
)
