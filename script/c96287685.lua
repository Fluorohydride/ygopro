--瓶亀
function c96287685.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c96287685.regop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c96287685.drawop)
	c:RegisterEffect(e2)
end
function c96287685.regop(e,tp,eg,ep,ev,re,r,rp)
	if re:GetHandler():IsCode(83968380) then
		e:GetHandler():RegisterFlagEffect(96287685,RESET_EVENT+0x1fe0000+RESET_CHAIN,0,1)
	end
end
function c96287685.drawop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(96287685)~=0 and re:GetHandler():IsCode(83968380) then
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
