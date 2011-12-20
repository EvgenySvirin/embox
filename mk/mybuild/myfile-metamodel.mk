# Generated by Xpand using M2Make template.

# Meta model for 'myFile' package.

ifndef __mybuild_myfile_metamodel_mk
__mybuild_myfile_metamodel_mk := $(lastword $(MAKEFILE_LIST))

include $(dir $(__mybuild_myfile_metamodel_mk))myfile-metamodel_impl.mk

# Provides static access to contents of 'myFile' model.
# See 'myFileMetaModelInstance'.
define class-MyFileMetaModel
	$(super MetaModel)

	$(field Model : MetaClass)
	$(field Model_Package : MetaReference)
	$(field Model_Imports : MetaReference)
	$(field Model_Entities : MetaReference)

	$(field Package : MetaClass)
	$(field Package_Name : MetaAttribute)

	$(field Import : MetaClass)
	$(field Import_ImportedNamespace : MetaAttribute)

	$(field Entity : MetaClass)
	$(field Entity_Name : MetaAttribute)

	$(field Interface : MetaClass)
	$(field Interface_SuperType : MetaReference)
	$(field Interface_Features : MetaReference)
	$(field Interface_SubTypes : MetaReference)

	$(field Feature : MetaClass)
	$(field Feature_Name : MetaAttribute)
	$(field Feature_SuperType : MetaReference)
	$(field Feature_Interface : MetaReference)
	$(field Feature_ProvidedBy : MetaReference)
	$(field Feature_RequiredBy : MetaReference)
	$(field Feature_SubTypes : MetaReference)

	$(field Module : MetaClass)
	$(field Module_Static : MetaAttribute)
	$(field Module_Abstract : MetaAttribute)
	$(field Module_SuperType : MetaReference)
	$(field Module_SubTypes : MetaReference)
	$(field Module_Depends : MetaReference)
	$(field Module_Dependent : MetaReference)
	$(field Module_Provides : MetaReference)
	$(field Module_Requires : MetaReference)

	$(field Filename : MetaClass)
	$(field Filename_Name : MetaAttribute)
endef

$(def_all)

myFileMetaModelInstance := $(call new,MyFileMetaModelImpl)

endif # __mybuild_myfile_metamodel_mk

