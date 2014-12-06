--エレキリギリス
function c61380658.initial_effect(c)
	--untargetable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetTarget(c61380658.target)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e2:SetValue(c61380658.val)
	c:RegisterEffect(e2)
end
function c61380658.target(e,c)
	return c:IsSetCard(0xe) and c~=e:GetHandler()
end
function c61380658.val(e,re,rp)
	return rp~=e:GetOwnerPlayer()
end
