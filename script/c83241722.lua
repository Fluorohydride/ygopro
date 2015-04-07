--リバースダイス
function c83241722.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c83241722.regop)
	c:RegisterEffect(e1)
end
function c83241722.regop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TOSS_DICE_NEGATE)
	e1:SetCondition(c83241722.coincon)
	e1:SetOperation(c83241722.coinop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c83241722.coincon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,83241722)==0
end
function c83241722.coinop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFlagEffect(tp,83241722)~=0 then return end
	if Duel.SelectYesNo(tp,aux.Stringid(83241722,0)) then
		Duel.Hint(HINT_CARD,0,83241722)
		Duel.RegisterFlagEffect(tp,83241722,RESET_PHASE+PHASE_END,0,1)
		local ct1=bit.band(ev,0xff)
		local ct2=bit.rshift(ev,16)
		Duel.TossDice(ep,ct1,ct2)
	end
end
