--慧眼の魔術師
function c72714461.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--set
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(72714461,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_PZONE)
	e2:SetCondition(c72714461.setcon)
	e2:SetTarget(c72714461.settg)
	e2:SetOperation(c72714461.setop)
	c:RegisterEffect(e2)
	--scale change
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(72714461,1))
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetRange(LOCATION_HAND)
	e3:SetCost(c72714461.sccost)
	e3:SetTarget(c72714461.sctg)
	e3:SetOperation(c72714461.scop)
	c:RegisterEffect(e3)
end
function c72714461.setcon(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	local tc=Duel.GetFieldCard(e:GetHandlerPlayer(),LOCATION_SZONE,13-seq)
	return tc and (tc:IsSetCard(0x98) or tc:IsSetCard(0x9f))
end
function c72714461.setfilter(c)
	return c:IsSetCard(0x98) and c:IsType(TYPE_PENDULUM) and not c:IsCode(72714461) and not c:IsForbidden()
end
function c72714461.settg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsDestructable()
		and Duel.IsExistingMatchingCard(c72714461.setfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetHandler(),1,0,0)
end
function c72714461.setop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or Duel.Destroy(c,REASON_EFFECT)==0 then return end
	local seq=c:GetSequence()
	if Duel.GetFieldCard(tp,LOCATION_SZONE,13-seq)~=nil then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOFIELD)
	local g=Duel.SelectMatchingCard(tp,c72714461.setfilter,tp,LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	if tc then
		Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
	end
end
function c72714461.sccost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsDiscardable() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST+REASON_DISCARD)
end
function c72714461.filter(c)
	return c:IsFaceup() and c:GetLeftScale()~=c:GetOriginalLeftScale() and c:GetRightScale()~=c:GetOriginalRightScale() and (c:GetSequence()==6 or c:GetSequence()==7)
end
function c72714461.sctg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c72714461.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c72714461.filter,tp,LOCATION_SZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c72714461.filter,tp,LOCATION_SZONE,0,1,1,nil)
end
function c72714461.scop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LSCALE)
		e1:SetValue(tc:GetOriginalLeftScale())
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CHANGE_RSCALE)
		e2:SetValue(tc:GetOriginalRightScale())
		tc:RegisterEffect(e2)
	end
end
