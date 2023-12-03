project(QtGraph)

set(QTGRAPH_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/nodefactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/idgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/focuswatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/helpers/keywatcher.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/logics/typemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/logics/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/logics/pin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/logics/graph.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/nodespawndata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/pindata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/models/pindragsignal.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/nfbuttonminimize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typebrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/canvas.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typednodeimage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/pin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/typednode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/customnode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/widgets/nodeimage.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/utilities/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/utilities/utility.h

    ${CMAKE_CURRENT_SOURCE_DIR}/private/qtgraph.h

    PARENT_SCOPE
)
