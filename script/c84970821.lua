--暗黒の呪縛
function c84970821.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_CHAIN_SOLVED)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c84970821.op)
	c:RegisterEffect(e2)
end
function c84970821.op(e,tp,eg,ep,ev,re,r,rp)
	local te,cp=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_EFFECT,CHAININFO_TRIGGERING_PLAYER)
	local c=te:GetHandler()
	if te:IsHasType(EFFECT_TYPE_ACTIVATE) and c:IsType(TYPE_SPELL) and c~=e:GetHandler() then
		Duel.Damage(cp,1000,REASON_EFFECT)
	end
end
