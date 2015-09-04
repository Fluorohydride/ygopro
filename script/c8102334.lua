--ゲート・ブロッカー
function c8102334.initial_effect(c)
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_SZONE)
	e1:SetTarget(c8102334.distg)
	c:RegisterEffect(e1)
	--disable effect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c8102334.disop)
	c:RegisterEffect(e2)
	--prevent counter
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_CANNOT_PLACE_COUNTER)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(0,1)
	c:RegisterEffect(e3)
	--target
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_MZONE)
	e4:SetTargetRange(0,0xff)
	e4:SetCode(EFFECT_CANNOT_SELECT_EFFECT_TARGET)
	e4:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e4:SetValue(c8102334.tglimit)
	c:RegisterEffect(e4)
end
function c8102334.distg(e,c)
	return c:IsType(TYPE_FIELD)
end
function c8102334.disop(e,tp,eg,ep,ev,re,r,rp)
	local tl=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if tl==LOCATION_SZONE and re:IsActiveType(TYPE_FIELD) and tp~=rp then
		Duel.NegateEffect(ev)
	end
end
function c8102334.tglimit(e,re,c)
	return c~=e:GetHandler() and c:IsControler(e:GetHandlerPlayer()) and c:IsLocation(LOCATION_MZONE)
end
