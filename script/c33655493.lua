--闇の侯爵ベリアル
function c33655493.initial_effect(c)
	--cannot be target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e1:SetValue(c33655493.tg)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_SELECT_EFFECT_TARGET)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e2:SetTargetRange(0,0xff)
	e2:SetValue(c33655493.tglimit)
	c:RegisterEffect(e2)
end
function c33655493.tg(e,c)
	return c:IsFaceup() and c:GetCode()~=33655493
end
function c33655493.tglimit(e,re,c)
	return re:IsActiveType(TYPE_SPELL+TYPE_TRAP) and c:IsControler(e:GetHandlerPlayer()) and c:IsLocation(LOCATION_MZONE) 
		and c:IsFaceup() and c:GetCode()~=33655493
end
