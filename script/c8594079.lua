--ジュラック・ブラキス
function c8594079.initial_effect(c)
	--battle indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetCondition(c8594079.indcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c8594079.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x22)
end
function c8594079.indcon(e)
	return Duel.IsExistingMatchingCard(c8594079.filter,0,LOCATION_MZONE,LOCATION_MZONE,1,e:GetHandler())
end
