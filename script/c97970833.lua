--ラスト・リゾート
function c97970833.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c97970833.condition)
	e1:SetTarget(c97970833.target)
	e1:SetOperation(c97970833.activate)
	c:RegisterEffect(e1)
end
function c97970833.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c97970833.filter(c,tp)
	return c:IsCode(34487429) and c:GetActivateEffect():IsActivatable(tp)
end
function c97970833.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c97970833.filter,tp,LOCATION_DECK,0,1,nil,tp) end
end
function c97970833.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstMatchingCard(c97970833.filter,tp,LOCATION_DECK,0,nil,tp)
	if tc then
		local fc=Duel.GetFieldCard(tp,LOCATION_SZONE,5)
		if fc then
			Duel.SendtoGrave(fc,REASON_RULE)
			Duel.BreakEffect()
		end
		Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
		fc=Duel.GetFieldCard(1-tp,LOCATION_SZONE,5)
		if fc and fc:IsFaceup() and Duel.IsPlayerCanDraw(1-tp,1) and Duel.SelectYesNo(tp,aux.Stringid(97970833,0)) then
			Duel.Draw(1-tp,1,REASON_EFFECT)
		end
		Duel.RaiseEvent(tc,EVENT_CHAIN_SOLVED,tc:GetActivateEffect(),0,tp,tp,Duel.GetCurrentChain())
	end
end
