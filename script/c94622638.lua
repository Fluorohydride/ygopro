--A・ジェネクス・パワーコール
function c94622638.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c94622638.atktg)
	e1:SetValue(500)
	c:RegisterEffect(e1)
end
function c94622638.atktg(e,c)
	return c~=e:GetHandler() and e:GetHandler():IsAttribute(c:GetAttribute())
end
