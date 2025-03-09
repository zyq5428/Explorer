# chart

## lv_example_chart_7

注释如下：

``` C {.line-numbers highlight=[]}
#include "../../lv_examples.h" // 包含 LittlevGL 示例相关的头文件
#if LV_USE_CHART && LV_BUILD_EXAMPLES // 确保启用了图表组件并且正在构建示例

// 静态函数，用于处理图表的绘制事件
static void draw_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e); // 获取绘制任务
    lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task); // 获取绘制描述符

    // 检查绘制的部分是否是 LV_PART_INDICATOR (图表上的数据点)
    if(base_dsc->part == LV_PART_INDICATOR) {
        lv_obj_t * obj = lv_event_get_target(e); // 获取事件的目标对象 (图表对象)
        lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL); // 获取图表的第一个序列 (series)
        lv_draw_rect_dsc_t * rect_draw_dsc = lv_draw_task_get_draw_dsc(draw_task); // 获取矩形绘制描述符
        uint32_t cnt = lv_chart_get_point_count(obj); // 获取图表中的数据点数量

        /*Make older value more transparent*/
        // 根据数据点的绘制顺序 (base_dsc->id2) 设置透明度，使较旧的点更透明
        rect_draw_dsc->bg_opa = (LV_OPA_COVER * base_dsc->id2) / (cnt - 1);

        /*Make smaller values blue, higher values red*/
        // 获取 X 和 Y 轴的数据数组
        int32_t * x_array = lv_chart_get_x_array(obj, ser);
        int32_t * y_array = lv_chart_get_y_array(obj, ser);

        // 获取图表序列的起始点索引
        uint32_t start_point = lv_chart_get_x_start_point(obj, ser);
        // 计算当前绘制的数据点在数组中的实际索引，考虑了起始点
        uint32_t p_act = (start_point + base_dsc->id2) % cnt;

        // 根据 X 和 Y 轴的值计算透明度，用于颜色混合
        lv_opa_t x_opa = (x_array[p_act] * LV_OPA_50) / 200;
        lv_opa_t y_opa = (y_array[p_act] * LV_OPA_50) / 1000;

        // 根据 X 和 Y 轴的值混合红色和蓝色，设置数据点的颜色
        rect_draw_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_RED),
                                               lv_palette_main(LV_PALETTE_BLUE),
                                               x_opa + y_opa);
    }
}

// 静态函数，用于向图表添加随机数据
static void add_data(lv_timer_t * timer)
{
    lv_obj_t * chart = lv_timer_get_user_data(timer); // 获取定时器的用户数据 (图表对象)
    // 向图表添加新的随机数据点
    lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(0, 200), lv_rand(0, 1000));
}

/**
 * A scatter chart (散点图)
 */
void lv_example_chart_7(void)
{
    lv_obj_t * chart = lv_chart_create(lv_screen_active()); // 创建一个图表对象
    lv_obj_set_size(chart, 200, 150); // 设置图表的大小
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0); // 将图表居中对齐
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL); // 添加绘制事件回调函数
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS); // 启用发送绘制任务事件
    lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);   /*Remove the lines*/ // 移除数据点之间的连线

    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER); // 设置图表类型为散点图

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 200); // 设置 X 轴的范围
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000); // 设置 Y 轴的范围

    lv_chart_set_point_count(chart, 50); // 设置图表中数据点的最大数量

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y); // 添加一个序列，颜色为红色，Y 轴为主轴
    uint32_t i;
    // 初始化图表，添加 50 个随机数据点
    for(i = 0; i < 50; i++) {
        lv_chart_set_next_value2(chart, ser, lv_rand(0, 200), lv_rand(0, 1000));
    }

    lv_timer_create(add_data, 100, chart); // 创建一个定时器，每 100 毫秒调用 add_data 函数，并将图表对象作为用户数据传递
}

#endif // LV_USE_CHART && LV_BUILD_EXAMPLES

```

