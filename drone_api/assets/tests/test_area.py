import sys
import os
# libディレクトリをモジュール検索パスに追加
sys.path.append(os.path.join(os.path.dirname(__file__), '../lib'))

import unittest
from lib.hako_area_accessor_impl import HakoAreaAccessorImpl
from lib.hako_aabb_object_space import HakoAABBObjectSpace

class TestHakoAreaAccessor(unittest.TestCase):

    def setUp(self):
        # テスト用のJSONファイルのパスを指定
        self.json_file_path = 'tests/test_data/area.json'
        self.area_accessor = HakoAreaAccessorImpl(self.json_file_path)

    def test_load_area_data(self):
        """エリアデータが正しく読み込まれるかを確認"""
        # area_id "area_1" のデータが正しく読み込まれたか確認
        area_1_bounds = self.area_accessor.space_areas.get('area_1')
        self.assertIsNotNone(area_1_bounds, "area_1のデータが存在しません。")
        self.assertEqual(area_1_bounds['min'], (0, 0, 0))
        self.assertEqual(area_1_bounds['max'], (100, 100, 50))

    def test_area_id_retrieval(self):
        """オブジェクトが正しいエリアに属するかを確認"""
        # オブジェクトがエリア "area_1" に属するかテスト
        object_space = HakoAABBObjectSpace(position=(50, 50, 25), size=(10, 10, 10))
        area_id = self.area_accessor.get_area_id(object_space)
        self.assertEqual(area_id, "area_1")

        # オブジェクトがエリア "area_2" に属するかテスト
        object_space = HakoAABBObjectSpace(position=(150, 50, 25), size=(10, 10, 10))
        area_id = self.area_accessor.get_area_id(object_space)
        self.assertEqual(area_id, "area_2")

    def test_outside_area(self):
        """オブジェクトがどのエリアにも属さない場合"""
        object_space = HakoAABBObjectSpace(position=(250, 250, 25), size=(10, 10, 10))
        area_id = self.area_accessor.get_area_id(object_space)
        self.assertIsNone(area_id, "エリア外のオブジェクトがエリア内と判定されました。")

if __name__ == '__main__':
    unittest.main()
