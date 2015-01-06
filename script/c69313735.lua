--ディスカバード・アタック
function c69313735.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c69313735.cost)
	e1:SetTarget(c69313735.target)
	e1:SetOperation(c69313735.activate)
	c:RegisterEffect(e1)
end
function c69313735.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c69313735.rfilter(c,tp)
	return c:IsSetCard(0x45) and Duel.IsExistingTarget(c69313735.filter,tp,LOCATION_MZONE,0,1,c)
end
function c69313735.filter(c)
	return c:IsFaceup() and c:IsCode(35975813)
end
function c69313735.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c69313735.filter(chkc) end
	if chk==0 then
		if e:GetLabel()==0 then return Duel.IsExistingTarget(c69313735.filter,tp,LOCATION_MZONE,0,1,nil) end
		e:SetLabel(0)
		return Duel.CheckReleaseGroup(tp,c69313735.rfilter,1,nil,tp)
	end
	if e:GetLabel()~=0 then
		e:SetLabel(0)
		local rg=Duel.SelectReleaseGroup(tp,c69313735.rfilter,1,1,nil,tp)
		Duel.Release(rg,REASON_COST)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c69313735.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c69313735.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
