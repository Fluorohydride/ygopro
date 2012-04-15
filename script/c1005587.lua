--煉獄の落とし穴
function c1005587.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetTarget(c1005587.target)
	e1:SetOperation(c1005587.activate)
	c:RegisterEffect(e1)
end
function c1005587.filter(c,tp)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT) and c:IsAttackAbove(2000) and c:GetSummonPlayer()==tp and c:IsDestructable()
end
function c1005587.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return eg:FilterCount(c1005587.filter,nil,1-tp)==1 end
	local g=eg:Filter(c1005587.filter,nil,1-tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c1005587.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsAttackAbove(2000) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_CHAIN_SOLVING)
		e2:SetOperation(c1005587.disop)
		e2:SetLabelObject(tc)
		e2:SetReset(RESET_CHAIN)
		Duel.RegisterEffect(e2,tp)
		Duel.AdjustInstantly()
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c1005587.disop(e,tp,eg,ep,ev,re,r,rp)
	if re:GetHandler()==e:GetLabelObject() then
		Duel.NegateEffect(ev)
	end
end
