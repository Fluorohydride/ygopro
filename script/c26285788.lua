--招来の対価
function c26285788.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,26285788+EFFECT_COUNT_CODE_OATH)
	e1:SetOperation(c26285788.activate)
	c:RegisterEffect(e1)
	if c26285788.global_effect==nil then
		c26285788.global_effect=true
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e1:SetCode(EVENT_RELEASE)
		e1:SetOperation(c26285788.addcount)
		Duel.RegisterEffect(e1,0)
	end
end
function c26285788.addcount(e,tp,eg,ep,ev,re,r,rp)
	local c=eg:GetFirst()
	while c~=nil do
		if not c:IsType(TYPE_TOKEN) then
			local p=c:GetReasonPlayer()
			Duel.RegisterFlagEffect(p,26285789,RESET_PHASE+PHASE_END,0,1)
		end
		c=eg:GetNext()
	end
end
function c26285788.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetCondition(c26285788.effectcon)
	e1:SetOperation(c26285788.effectop)
	Duel.RegisterEffect(e1,tp)
end
function c26285788.effectcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,26285789)>0
end
function c26285788.filter1(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c26285788.filter2(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c26285788.effectop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_CARD,0,26285788)
	local ct=Duel.GetFlagEffect(tp,26285789)
	if ct==1 then
		Duel.Draw(tp,1,REASON_EFFECT)
	elseif ct==2 then
		local g=Duel.GetMatchingGroup(c26285788.filter1,tp,LOCATION_GRAVE,0,nil)
		if g:GetCount()>1 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local tg=g:Select(tp,2,2,nil)
			Duel.SendtoHand(tg,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,tg)
		end
	else
		local g=Duel.GetMatchingGroup(c26285788.filter2,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		if g:GetCount()>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local tg=g:Select(tp,1,3,nil)
			Duel.Destroy(tg,REASON_EFFECT)
		end
	end
end
