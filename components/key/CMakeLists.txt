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

# 私有依赖(仅在 .c 内部使用, 不对外暴露)
set(COMPONENT_PRIV_REQUIRES
)

# 注册当前组件
idf_component_register(
    SRC_DIRS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
    REQUIRES ${COMPONENT_REQUIRES}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
)
