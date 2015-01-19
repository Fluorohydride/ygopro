--インフェルニティ・ドワーフ
function c25171661.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCondition(c25171661.condition)
	c:RegisterEffect(e1)
end
function c25171661.condition(e)
	return Duel.GetFieldGroupCount(e:GetHandler():GetControler(),LOCATION_HAND,0)==0
end
