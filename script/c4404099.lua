--海底に潜む深海竜
function c4404099.initial_effect(c)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(4404099,0))
	e1:SetCategory(CATEGORY_COUNTER)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetTarget(c4404099.addct)
	e1:SetOperation(c4404099.addc)
	c:RegisterEffect(e1)
	--attackup
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(4404099,1))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetCondition(c4404099.atkcon)
	e2:SetOperation(c4404099.atkop)
	c:RegisterEffect(e2)
end
function c4404099.addct(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_COUNTER,nil,1,0,0x23)
end
function c4404099.addc(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		e:GetHandler():AddCounter(0x23,1)
	end
end
function c4404099.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetHandler():GetCounter(0x23)
	e:SetLabel(ct)
	return ct>0 and not e:GetHandler():IsLocation(LOCATION_DECK)
end
function c4404099.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_FISH+RACE_SEASERPENT)
end
function c4404099.atkop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c4404099.filter,tp,LOCATION_MZONE,0,nil)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(e:GetLabel()*200)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end
