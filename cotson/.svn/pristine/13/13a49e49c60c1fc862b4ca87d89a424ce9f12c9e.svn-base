# (C) Copyright 2009 Hewlett-Packard Development Company, L.P.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

# $Id: 014_create_sandbox_models.rb 6977 2009-12-09 16:18:35Z frb $

class CreateSandboxModels < ActiveRecord::Migration
  def self.up
    create_table :sandbox_models do |t|
      t.string :sandbox
      t.text :configuration
      t.boolean :mediator
      t.integer :node_id
      t.string :mediator_data

      t.timestamps
    end
  end

  def self.down
    drop_table :sandbox_models
  end
end
