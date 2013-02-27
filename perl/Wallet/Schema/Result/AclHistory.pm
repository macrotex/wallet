package Wallet::Schema::Result::AclHistory;

use strict;
use warnings;

use base 'DBIx::Class::Core';

__PACKAGE__->load_components("InflateColumn::DateTime");

=for stopwords
ACL

=head1 NAME

Wallet::Schema::Result::AclHistory - Wallet schema for ACL history

=head1 DESCRIPTION

=cut

__PACKAGE__->table("acl_history");

=head1 ACCESSORS

=head2 ah_id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 ah_acl

  data_type: 'integer'
  is_nullable: 0

=head2 ah_action

  data_type: 'varchar'
  is_nullable: 0
  size: 16

=head2 ah_scheme

  data_type: 'varchar'
  is_nullable: 1
  size: 32

=head2 ah_identifier

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 ah_by

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=head2 ah_from

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=head2 ah_on

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "ah_id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "ah_acl",
  { data_type => "integer", is_nullable => 0 },
  "ah_action",
  { data_type => "varchar", is_nullable => 0, size => 16 },
  "ah_scheme",
  { data_type => "varchar", is_nullable => 1, size => 32 },
  "ah_identifier",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "ah_by",
  { data_type => "varchar", is_nullable => 0, size => 255 },
  "ah_from",
  { data_type => "varchar", is_nullable => 0, size => 255 },
  "ah_on",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 0,
  },
);
__PACKAGE__->set_primary_key("ah_id");

__PACKAGE__->might_have(
                        'acls',
                        'Wallet::Schema::Result::Acl',
                        { 'foreign.ac_id' => 'self.ah_id' },
                       );

1;
