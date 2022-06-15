#===============================================================================
# 10-reliable (reliable client/server application)
#===============================================================================

- reliable client/server mesh communication by repeated sending of mesh messages
- repeating of mesh messages is done by bl_mpub module (mesh publisher), which
  multiply schedules mesh commands with different delays

# Lessons to Learn

- how to use the bl_mpub() function
- how to send enhanced generic on/off commands utilizing the data reference part
  of the [GONOFF:SET @id,<BL_goo>,onoff] command
- how to define transition objects
- how to deal with transitions using bl_trans(), bl_cur() and bl_fin()
