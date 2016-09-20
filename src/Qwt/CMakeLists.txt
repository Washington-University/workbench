################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

#
# Name of project
#
PROJECT (Qwt)

#
# QT include files
#
if(Qt5_FOUND)
    include_directories(${Qt5Concurrent_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

add_definitions(-DQWT_NO_SVG -DQWT_NO_OPENGL)

#
# Header files
#
SET(SOURCE_FILES
qwt_abstract_legend.h
qwt_abstract_scale_draw.h
qwt_clipper.h
qwt_color_map.h
qwt_column_symbol.h
qwt_compat.h
qwt_curve_fitter.h
qwt_date.h
qwt_date_scale_draw.h
qwt_date_scale_engine.h
qwt_dyngrid_layout.h
qwt_event_pattern.h
qwt_global.h
qwt_graphic.h
qwt.h
qwt_interval.h
qwt_interval_symbol.h
qwt_legend_data.h
qwt_legend.h
qwt_legend_label.h
qwt_magnifier.h
qwt_math.h
qwt_matrix_raster_data.h
qwt_null_paintdevice.h
qwt_painter_command.h
qwt_painter.h
qwt_panner.h
qwt_picker.h
qwt_picker_machine.h
qwt_pixel_matrix.h
qwt_plot_abstract_barchart.h
qwt_plot_barchart.h
qwt_plot_canvas.h
qwt_plot_curve.h
qwt_plot_dict.h
qwt_plot_directpainter.h
qwt_plot_grid.h
qwt_plot.h
qwt_plot_histogram.h
qwt_plot_intervalcurve.h
qwt_plot_item.h
qwt_plot_layout.h
qwt_plot_legenditem.h
qwt_plot_magnifier.h
qwt_plot_marker.h
qwt_plot_multi_barchart.h
qwt_plot_panner.h
qwt_plot_picker.h
qwt_plot_rasteritem.h
qwt_plot_renderer.h
qwt_plot_rescaler.h
qwt_plot_scaleitem.h
qwt_plot_seriesitem.h
qwt_plot_shapeitem.h
qwt_plot_spectrocurve.h
qwt_plot_spectrogram.h
qwt_plot_textlabel.h
qwt_plot_tradingcurve.h
qwt_plot_zoneitem.h
qwt_plot_zoomer.h
qwt_point_3d.h
qwt_point_data.h
qwt_point_mapper.h
qwt_point_polar.h
qwt_raster_data.h
qwt_round_scale_draw.h
qwt_samples.h
qwt_sampling_thread.h
qwt_scale_div.h
qwt_scale_draw.h
qwt_scale_engine.h
qwt_scale_map.h
qwt_scale_widget.h
qwt_series_data.h
qwt_series_store.h
qwt_spline.h
qwt_symbol.h
qwt_system_clock.h
qwt_text_engine.h
qwt_text.h
qwt_text_label.h
qwt_transform.h
qwt_widget_overlay.h

qwt_abstract_legend.cpp
qwt_abstract_scale_draw.cpp
qwt_clipper.cpp
qwt_color_map.cpp
qwt_column_symbol.cpp
qwt_curve_fitter.cpp
qwt_date.cpp
qwt_date_scale_draw.cpp
qwt_date_scale_engine.cpp
qwt_dyngrid_layout.cpp
qwt_event_pattern.cpp
qwt_graphic.cpp
qwt_interval.cpp
qwt_interval_symbol.cpp
qwt_legend.cpp
qwt_legend_data.cpp
qwt_legend_label.cpp
qwt_magnifier.cpp
qwt_math.cpp
qwt_matrix_raster_data.cpp
qwt_null_paintdevice.cpp
qwt_painter_command.cpp
qwt_painter.cpp
qwt_panner.cpp
qwt_picker.cpp
qwt_picker_machine.cpp
qwt_pixel_matrix.cpp
qwt_plot_abstract_barchart.cpp
qwt_plot_axis.cpp
qwt_plot_barchart.cpp
qwt_plot_canvas.cpp
qwt_plot.cpp
qwt_plot_curve.cpp
qwt_plot_dict.cpp
qwt_plot_directpainter.cpp
qwt_plot_grid.cpp
qwt_plot_histogram.cpp
qwt_plot_intervalcurve.cpp
qwt_plot_item.cpp
qwt_plot_layout.cpp
qwt_plot_legenditem.cpp
qwt_plot_magnifier.cpp
qwt_plot_marker.cpp
qwt_plot_multi_barchart.cpp
qwt_plot_panner.cpp
qwt_plot_picker.cpp
qwt_plot_rasteritem.cpp
qwt_plot_renderer.cpp
qwt_plot_rescaler.cpp
qwt_plot_scaleitem.cpp
qwt_plot_seriesitem.cpp
qwt_plot_shapeitem.cpp
qwt_plot_spectrocurve.cpp
qwt_plot_spectrogram.cpp
qwt_plot_textlabel.cpp
qwt_plot_tradingcurve.cpp
qwt_plot_xml.cpp
qwt_plot_zoneitem.cpp
qwt_plot_zoomer.cpp
qwt_point_3d.cpp
qwt_point_data.cpp
qwt_point_mapper.cpp
qwt_point_polar.cpp
qwt_raster_data.cpp
qwt_round_scale_draw.cpp
qwt_sampling_thread.cpp
qwt_scale_div.cpp
qwt_scale_draw.cpp
qwt_scale_engine.cpp
qwt_scale_map.cpp
qwt_scale_widget.cpp
qwt_series_data.cpp
qwt_spline.cpp
qwt_symbol.cpp
qwt_system_clock.cpp
qwt_text.cpp
qwt_text_engine.cpp
qwt_text_label.cpp
qwt_transform.cpp
qwt_widget_overlay.cpp
)

#
# Process the header files with moc producing moc_*.cpp files
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_AUTOMOC ON)

#
# Create the GUI library
#
ADD_LIBRARY(Qwt
${SOURCE_FILES}
)

SET(Qwt_LIBRARIES Qwt PARENT_SCOPE)
SET(Qwt_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
