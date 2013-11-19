--悲劇の引き金
function c61656650.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c61656650.efcon)
	e1:SetTarget(c61656650.eftg)
	e1:SetOperation(c61656650.efop)
	c:RegisterEffect(e1)
end
function c61656650.efcon(e,tp,eg,ep,ev,re,r,rp)
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	if not g or g:GetCount()~=1 then return false end
	local tc=g:GetFirst()
	local ex,dg=Duel.GetOperationInfo(0,CATEGORY_DESTROY)
	return tc:IsControler(tp) and dg and dg:GetCount()==1 and dg:GetFirst()==tc and tc:IsType(TYPE_MONSTER)
end
function c61656650.filter(c,re,rp,tf)
	return tf(re,rp,nil,nil,nil,nil,nil,nil,0,c)
end
function c61656650.eftg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local tf=re:GetTarget()
	if chk==0 then return Duel.IsExistingTarget(c61656650.filter,tp,0,LOCATION_MZONE,1,nil,re,rp,tf) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c61656650.filter,tp,0,LOCATION_MZONE,1,1,nil,re,rp,tf)
end
function c61656650.efop(e,tp,eg,ep,ev,re,r,rp)
	local tf=re:GetTarget()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tf(re,rp,nil,nil,nil,nil,nil,nil,0,tc) then
		local g=Group.FromCards(tc)
		Duel.ChangeTargetCard(ev,g)
	end
end
