--至高の木の実
function c98380593.initial_effect(c)
	--recover
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_RECOVER+CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c98380593.rectg)
	e1:SetOperation(c98380593.recop)
	c:RegisterEffect(e1)
end
function c98380593.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.GetLP(tp)~=Duel.GetLP(1-tp) end
	Duel.SetTargetPlayer(tp)
	if Duel.GetLP(tp)<Duel.GetLP(1-tp) then
		e:SetLabel(0)
		Duel.SetTargetParam(2000)
		Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,2000)
	else
		e:SetLabel(1)
		Duel.SetTargetParam(1000)
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,tp,1000)
	end
end
function c98380593.recop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	if e:GetLabel()==0 then
		Duel.Recover(p,d,REASON_EFFECT)
	else
		Duel.Damage(p,d,REASON_EFFECT)
	end
end
