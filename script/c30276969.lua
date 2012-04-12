--氷弾使いレイス
function c30276969.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c30276969.indes)
	c:RegisterEffect(e1)
end
function c30276969.indes(e,c)
	return c:IsLevelAbove(4)
end
