require 'gr/object'
require 'gr/callback'

class TestObject < GCore::Object
  native 'TestObject'

  def call_cb
    callback.inv "InRuby"
  end

end