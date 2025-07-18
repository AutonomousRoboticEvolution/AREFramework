local codeEditorInfos=[[
int result = simUI.msgBox(int type, int buttons, string title, string message)
string[] result = simUI.fileDialog(int type, string title, string startPath, string initName, string extName, string ext, bool native=false)
float[3] result = simUI.colorDialog(float[3..4] initColor={1,1,1}, string title="Select color", bool showAlphaChannel=false, bool native=true)
string result = simUI.inputDialog(string initValue="", string label="Input value:", string title="")
string uiHandle = simUI.create(string xml)
simUI.destroy(string handle)
simUI.setButtonText(string handle, int id, string text)
simUI.setButtonPressed(string handle, int id, bool pressed)
int value = simUI.getSliderValue(string handle, int id)
simUI.setSliderValue(string handle, int id, int value, bool suppressEvents=true)
string value = simUI.getEditValue(string handle, int id)
simUI.setEditValue(string handle, int id, string value, bool suppressEvents=true)
float value = simUI.getSpinboxValue(string handle, int id)
simUI.setSpinboxValue(string handle, int id, float value, bool suppressEvents=true)
int value = simUI.getCheckboxValue(string handle, int id)
simUI.setCheckboxValue(string handle, int id, int value, bool suppressEvents=true)
int value = simUI.getRadiobuttonValue(string handle, int id)
simUI.setRadiobuttonValue(string handle, int id, int value, bool suppressEvents=true)
string text = simUI.getLabelText(string handle, int id)
simUI.setLabelText(string handle, int id, string text, bool suppressEvents=true)
simUI.insertComboboxItem(string handle, int id, int index, string text, bool suppressEvents=true)
simUI.removeComboboxItem(string handle, int id, int index, bool suppressEvents=true)
int count = simUI.getComboboxItemCount(string handle, int id)
string text = simUI.getComboboxItemText(string handle, int id, int index)
string[] items = simUI.getComboboxItems(string handle, int id)
simUI.setComboboxItems(string handle, int id, string[] items, int index, bool suppressEvents=true)
int index = simUI.getComboboxSelectedIndex(string handle, int id)
simUI.setComboboxSelectedIndex(string handle, int id, int index, bool suppressEvents=true)
simUI.adjustSize(string handle)
simUI.hide(string handle)
simUI.show(string handle)
bool visibility = simUI.isVisible(string handle)
int x, int y = simUI.getPosition(string handle)
simUI.setPosition(string handle, int x, int y, bool suppressEvents=true)
int w, int h = simUI.getSize(string handle)
simUI.setSize(string handle, int w, int h, bool suppressEvents=true)
string title = simUI.getTitle(string handle)
simUI.setTitle(string handle, string title, bool suppressEvents=true)
simUI.setWindowEnabled(string handle, bool enabled, bool suppressEvents=true)
simUI.setImageData(string handle, int id, buffer data, int width, int height)
simUI.setEnabled(string handle, int id, bool enabled, bool suppressEvents=true)
int index = simUI.getCurrentTab(string handle, int id)
simUI.setCurrentTab(string handle, int id, int index, bool suppressEvents=true)
simUI.setStyleSheet(string handle, int id, string styleSheet)
bool visibility = simUI.getWidgetVisibility(string handle, int id)
simUI.setWidgetVisibility(string handle, int id, bool visibility)
int id = simUI.getCurrentEditWidget(string handle)
simUI.setCurrentEditWidget(string handle, int id)
simUI.replot(string handle, int id)
simUI.addCurve(string handle, int id, int type, string name, int[3] color, int style, map options)
simUI.addCurveTimePoints(string handle, int id, string name, float[] x, float[] y)
simUI.addCurveXYPoints(string handle, int id, string name, float[] t, float[] x, float[] y)
simUI.clearCurve(string handle, int id, string name)
simUI.removeCurve(string handle, int id, string name)
simUI.setPlotRanges(string handle, int id, float xmin, float xmax, float ymin, float ymax)
simUI.setPlotXRange(string handle, int id, float xmin, float xmax)
simUI.setPlotYRange(string handle, int id, float ymin, float ymax)
simUI.growPlotRanges(string handle, int id, float xmin, float xmax, float ymin, float ymax)
simUI.growPlotXRange(string handle, int id, float xmin, float xmax)
simUI.growPlotYRange(string handle, int id, float ymin, float ymax)
simUI.setPlotLabels(string handle, int id, string x, string y)
simUI.setPlotXLabel(string handle, int id, string label)
simUI.setPlotYLabel(string handle, int id, string label)
simUI.rescaleAxes(string handle, int id, string name, bool onlyEnlargeX=false, bool onlyEnlargeY=false)
simUI.rescaleAxesAll(string handle, int id, bool onlyEnlargeX=false, bool onlyEnlargeY=false)
simUI.setMouseOptions(string handle, int id, bool panX, bool panY, bool zoomX, bool zoomY)
simUI.setLegendVisibility(string handle, int id, bool visible)
float[] t, float[] x, float[] y = simUI.getCurveData(string handle, int id, string name)
simUI.clearTable(string handle, int id, bool suppressEvents=true)
simUI.setRowCount(string handle, int id, int count, bool suppressEvents=true)
simUI.setColumnCount(string handle, int id, int count, bool suppressEvents=true)
simUI.setRowHeaderText(string handle, int id, int row, string text)
simUI.setColumnHeaderText(string handle, int id, int column, string text)
simUI.setItem(string handle, int id, int row, int column, string text, bool suppressEvents=true)
simUI.setItemImage(string handle, int id, int row, int column, string data, int width, int height, bool suppressEvents=true)
simUI.setItems(string handle, int id, string data, bool suppressEvents=true)
int count = simUI.getRowCount(string handle, int id)
int count = simUI.getColumnCount(string handle, int id)
string text = simUI.getItem(string handle, int id, int row, int column)
simUI.setItemEditable(string handle, int id, int row, int column, bool editable)
buffer state = simUI.saveState(string handle, int id)
simUI.restoreState(string handle, int id, buffer state)
simUI.setColumnWidth(string handle, int id, int column, int min_size, int max_size)
simUI.setRowHeight(string handle, int id, int row, int min_size, int max_size)
simUI.setTableSelection(string handle, int id, int row, int column, bool suppressEvents=true)
simUI.setProgress(string handle, int id, int value)
simUI.clearTree(string handle, int id, bool suppressEvents=true)
simUI.addTreeItem(string handle, int id, int item_id, string[] text, int parent_id=0, bool expanded=false, bool suppressEvents=true)
simUI.updateTreeItemText(string handle, int id, int item_id, string[] text)
simUI.updateTreeItemParent(string handle, int id, int item_id, int parent_id, bool suppressEvents=true)
simUI.removeTreeItem(string handle, int id, int item_id, bool suppressEvents=true)
simUI.setTreeSelection(string handle, int id, int item_id, bool suppressEvents=true)
simUI.expandAll(string handle, int id, bool suppressEvents=true)
simUI.collapseAll(string handle, int id, bool suppressEvents=true)
simUI.expandToDepth(string handle, int id, int depth, bool suppressEvents=true)
simUI.setText(string handle, int id, string text, bool suppressEvents=true)
simUI.appendText(string handle, int id, string text, bool suppressEvents=true)
simUI.setUrl(string handle, int id, string url)
simUI.addScene3DNode(string handle, int id, int nodeId, int parentNodeId, int type)
simUI.removeScene3DNode(string handle, int id, int nodeId)
simUI.setScene3DNodeEnabled(string handle, int id, int nodeId, bool enabled)
simUI.setScene3DNodeIntParam(string handle, int id, int nodeId, string paramName, int value)
simUI.setScene3DNodeFloatParam(string handle, int id, int nodeId, string paramName, float value)
simUI.setScene3DNodeStringParam(string handle, int id, int nodeId, string paramName, string value)
simUI.setScene3DNodeVector2Param(string handle, int id, int nodeId, string paramName, float x, float y)
simUI.setScene3DNodeVector3Param(string handle, int id, int nodeId, string paramName, float x, float y, float z)
simUI.setScene3DNodeVector4Param(string handle, int id, int nodeId, string paramName, float x, float y, float z, float w)
simUI.svgLoadFile(string handle, int id, string file)
simUI.svgLoadData(string handle, int id, string data)
string[] formatList, string formatListStr = simUI.supportedImageFormats(string separator=nil)
map m = simUI.getKeyboardModifiers()
int[] version = simUI.qtVersion()
simUI.setClipboardText(string text)
simUI.setProperties(string handle, int id, string[] pnames, string[] ptypes, string[] pvalues, int[] pflags, string[] pdisplayk, string[] pdisplayv, int[] icons, bool suppressEvents=true)
simUI.setPropertiesRows(string handle, int id, int[] rows, string[] pnames, string[] ptypes, string[] pvalues, int[] pflags, string[] pdisplayk, string[] pdisplayv, int[] icons, bool suppressEvents=true)
simUI.setPropertiesSelection(string handle, int id, int row, bool suppressEvents=true)
simUI.setPropertiesContextMenu(string handle, int id, string[] keys, string[] titles)
simUI.setPropertiesState(string handle, int id, string state)
string state = simUI.getPropertiesState(string handle, int id)
int id = simUI.bannerCreate(string text, string[] btnKeys=nil, string[] btnLabels=nil, string callback=nil)
simUI.bannerDestroy(int id)
simUI.insertTableRow(int ui, int widget, int index)
simUI.removeTableRow(int ui, int widget, int index)
simUI.insertTableColumn(int ui, int widget, int index)
simUI.removeTableColumn(int ui, int widget, int index)
simUI.setScene3DNodeParam(int ui, int widget, int nodeId, string paramName, any paramValue)
simUI.msgbox_type.info
simUI.msgbox_type.question
simUI.msgbox_type.warning
simUI.msgbox_type.critical
simUI.msgbox_buttons.ok
simUI.msgbox_buttons.yesno
simUI.msgbox_buttons.yesnocancel
simUI.msgbox_buttons.okcancel
simUI.msgbox_result.cancel
simUI.msgbox_result.no
simUI.msgbox_result.yes
simUI.msgbox_result.ok
simUI.filedialog_type.load
simUI.filedialog_type.load_multiple
simUI.filedialog_type.save
simUI.filedialog_type.folder
simUI.curve_type.time
simUI.curve_type.xy
simUI.curve_style.scatter
simUI.curve_style.line
simUI.curve_style.line_and_scatter
simUI.curve_style.step_left
simUI.curve_style.step_center
simUI.curve_style.step_right
simUI.curve_style.impulse
simUI.curve_scatter_shape.none
simUI.curve_scatter_shape.dot
simUI.curve_scatter_shape.cross
simUI.curve_scatter_shape.plus
simUI.curve_scatter_shape.circle
simUI.curve_scatter_shape.disc
simUI.curve_scatter_shape.square
simUI.curve_scatter_shape.diamond
simUI.curve_scatter_shape.star
simUI.curve_scatter_shape.triangle
simUI.curve_scatter_shape.triangle_inverted
simUI.curve_scatter_shape.cross_square
simUI.curve_scatter_shape.plus_square
simUI.curve_scatter_shape.cross_circle
simUI.curve_scatter_shape.plus_circle
simUI.curve_scatter_shape.peace
simUI.line_style.solid
simUI.line_style.dotted
simUI.line_style.dashed
simUI.scene3d_node_type.entity
simUI.scene3d_node_type.transform
simUI.scene3d_node_type.camera
simUI.scene3d_node_type.camera_controller_first_person
simUI.scene3d_node_type.camera_controller_orbit
simUI.scene3d_node_type.light_point
simUI.scene3d_node_type.light_directional
simUI.scene3d_node_type.light_spot
simUI.scene3d_node_type.mesh_cuboid
simUI.scene3d_node_type.mesh_torus
simUI.scene3d_node_type.mesh_cone
simUI.scene3d_node_type.mesh_cylinder
simUI.scene3d_node_type.mesh_plane
simUI.scene3d_node_type.mesh_sphere
simUI.scene3d_node_type.mesh
simUI.scene3d_node_type.material_phong
simUI.scene3d_node_type.material_gooch
simUI.scene3d_node_type.material_texture
simUI.scene3d_node_type.material_diffuse_map
simUI.scene3d_node_type.texture2d
simUI.scene3d_node_type.texture_image
simUI.scene3d_node_type.object_picker
simUI.mouse.left_button_down
simUI.mouse.left_button_up
simUI.mouse.move
]]

registerCodeEditorInfos("simUI",codeEditorInfos)
