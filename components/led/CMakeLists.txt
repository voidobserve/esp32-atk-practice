# 组件源码目录(路径)
set(COMPONENT_SRCS
    .
)

# 组件头文件目录(路径)
set(COMPONENT_ADD_INCLUDEDIRS
    .
)

# 依赖组件(公共依赖: 对外暴露的头文件里用到的)
set(COMPONENT_REQUIRES
    driver # esp32 driver component
)

# 注册当前组件
idf_component_register(
    SRC_DIRS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
    REQUIRES ${COMPONENT_REQUIRES}
)
