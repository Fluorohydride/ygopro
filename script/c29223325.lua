--アーティファクト・ムーブメント
function c29223325.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(29223325,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(0,TIMING_END_PHASE+TIMING_EQUIP)
	e1:SetTarget(c29223325.target)
	e1:SetOperation(c29223325.activate)
	c:RegisterEffect(e1)
	--skip
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(29223325,1))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetCondition(c29223325.descon)
	e2:SetOperation(c29223325.desop)
	c:RegisterEffect(e2)
end
function c29223325.filter(c)
	return c:IsSetCard(0x97) and c:IsType(TYPE_MONSTER) and c:IsSSetable(true)
end
function c29223325.desfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c29223325.desfilter2(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable() and c:GetSequence()<5
end
function c29223325.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c29223325.desfilter(chkc) and chkc~=e:GetHandler() end
	if chk==0 then
		if not Duel.IsExistingMatchingCard(c29223325.filter,tp,LOCATION_DECK,0,1,nil) then return false end
		local ft=Duel.GetLocationCount(tp,LOCATION_SZONE)
		if e:GetHandler():IsLocation(LOCATION_HAND) then ft=ft-1 end
		if ft<0 then return false
		elseif ft>0 then
			return Duel.IsExistingTarget(c29223325.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler())
		else
			return Duel.IsExistingTarget(c29223325.desfilter2,tp,LOCATION_ONFIELD,0,1,e:GetHandler())
		end
	end
	local ft=Duel.GetLocationCount(tp,LOCATION_SZONE)
	local g=nil
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	if ft>0 then
		g=Duel.SelectTarget(tp,c29223325.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
	else
		g=Duel.SelectTarget(tp,c29223325.desfilter2,tp,LOCATION_ONFIELD,0,1,1,e:GetHandler())
	end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c29223325.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
		local g=Duel.SelectMatchingCard(tp,c29223325.filter,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SSet(tp,g:GetFirst())
			Duel.ConfirmCards(1-tp,g)
		end
	end
end
function c29223325.descon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and e:GetHandler():GetPreviousControler()==tp
end
function c29223325.desop(e,tp,eg,ep,ev,re,r,rp)
	local ph=Duel.GetCurrentPhase()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SKIP_BP)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	if Duel.GetTurnPlayer()~=tp and ph>PHASE_MAIN1 and ph<PHASE_MAIN2 then
		e1:SetLabel(Duel.GetTurnCount())
		e1:SetCondition(c29223325.skipcon)
		e1:SetReset(RESET_PHASE+PHASE_BATTLE+RESET_OPPO_TURN,2)
	else
		e1:SetReset(RESET_PHASE+PHASE_BATTLE+RESET_OPPO_TURN,1)
	end
	Duel.RegisterEffect(e1,tp)
end
function c29223325.skipcon(e)
	return Duel.GetTurnCount()~=e:GetLabel()
end
