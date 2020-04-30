# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

GUI_JAVA_FILES=\
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/AggregateBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/AggregateBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/ArcEnd.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/AssignmentsSpecifier.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BinaryBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BinaryBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BinaryPredicateSpecifier.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxComponent.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxNodeInputPort.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxNodeOutputPort.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxPortConnectionPoint.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BoxesPanel.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BsortBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/BsortBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Callback.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/CatalogManager.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/CatalogManagerDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/CatalogNameDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Constants.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/DropBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/DropBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Editor.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/EmissionPolicySpecifier.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/FilterBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/FilterBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericArc.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericBoxNodePort.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericNodePropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericPort.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/GenericPortNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/HierarchyNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/HierarchyPanel.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/InputPort.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/InputPortNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/InputPortPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/JoinBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/JoinBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/MapBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/MapBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/MenuBar.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Messagespace.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Model.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/MultipleInputsBoxInterface.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/OutputPortNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/OutputPortNodePropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PalettePanel.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PortNodeDistributionDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PortNodePropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PortNodeRatePropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PortNodeVariancePropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PredicateSpecifier.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PrimitiveBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/PrimitiveBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/QosPoints.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/QosSpecifierException.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/QosSpecifiers.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/QosSpecifiersPanel.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/ReadRelationBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/ReadRelationBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/RemoteCatalogManager.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/RenamableLabel.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/ResampleBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/ResampleBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/RestreamBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/RestreamBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/RootFrame.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/SuperBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/SuperBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/TimeWindowSpecifier.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Toolbarspace.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Toolboxspace.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/TypeFailedBoxesWindow.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/TypeInferencer.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UnaryBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UnaryBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UnionBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UnionBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UpdateRelationBoxNode.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/UpdateRelationBoxPropertiesDialog.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Utils.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/WorkloadDataPackage.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/Workspace.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/ArcPaletteRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/ArcRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/BoxPaletteRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/BoxRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/CatalogListRecordDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/CompositeTypeRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/IntArrayDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/IntegerArrayDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/IntegerDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/MetadataRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/PaletteRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/PaletteUtils.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/PortPaletteRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/PortRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/RecordDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/StringDbt.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/dbts/TypeFieldRecord.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/AdhocAggregateFunction.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/AggregateFunction.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/Attribute.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/CompositeType.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/DistributionSet.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/Field.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/FieldSet.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/MessageBox.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/Parser.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/ParserVal.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/ParsingException.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/PrimitiveType.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/Type.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypeException.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypeList.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypeManager.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypePane.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypeTable.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/TypeTableDlg.java \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/UnnamedType.java

gui: \
	$(GUI_JAVA_FILES) \
	$(GUI_BASE_DIR)/build.properties \
	$(GUI_BASE_DIR)/build_gui_script
	$(GUI_BASE_DIR)/build_gui_script


$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/Parser.java \
$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/ParserVal.java : \
	$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/modifier.y \
	$(GUI_BYACCJ_APP)
	cd $(GUI_BASE_DIR)/edu/brown/aurora/gui/types && \
	$(GUI_BYACCJ_APP) -J -Jpackage=edu.brown.aurora.gui.types $(GUI_BASE_DIR)/edu/brown/aurora/gui/types/modifier.y

$(GUI_BASE_DIR)/build.properties: \
	$(AURORA_SRC_DIR)/makedefs_locations.mk
	@echo build.compiler=$(GUI_ANT_BUILD_COMPILER)     > $(GUI_BASE_DIR)/build.properties
	@echo catalog.path=$(CATALOG_MGR_BASE_DIR)        >> $(GUI_BASE_DIR)/build.properties
	@echo src.zip.dir=$(JAVA_SRC_PATH)                >> $(GUI_BASE_DIR)/build.properties
	@echo berkeleydb.lib.dir=$(BERKELEY_DB_4_LIB_DIR) >> $(GUI_BASE_DIR)/build.properties

$(GUI_BASE_DIR)/build_gui_script: \
	$(AURORA_SRC_DIR)/makedefs_locations.mk
	@echo \#!$(TCSH_PATH)                > $(GUI_BASE_DIR)/build_gui_script
	@echo setenv ANT_HOME $(ANT_HOME)   >> $(GUI_BASE_DIR)/build_gui_script
	@echo setenv JAVA_HOME $(JAVA_HOME) >> $(GUI_BASE_DIR)/build_gui_script
	@echo pushd $(GUI_BASE_DIR)         >> $(GUI_BASE_DIR)/build_gui_script
	@echo $(ANT_CMD)                    >> $(GUI_BASE_DIR)/build_gui_script
	@echo popd                          >> $(GUI_BASE_DIR)/build_gui_script
	@chmod ug+x $(GUI_BASE_DIR)/build_gui_script

clean_gui:
	rm -f $(GUI_BASE_DIR)/classes/edu/brown/aurora/gui/*.class
	rm -f $(GUI_BASE_DIR)/classes/edu/brown/aurora/gui/dbts/*.class
	rm -f $(GUI_BASE_DIR)/classes/edu/brown/aurora/gui/types/*.class
