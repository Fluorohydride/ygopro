--ライトニングパニッシャー
function c45023678.initial_effect(c)
	--chain
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c45023678.chop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(45023678,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_CHAIN_END)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c45023678.descon)
	e2:SetTarget(c45023678.destg)
	e2:SetOperation(c45023678.desop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c45023678.chop(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetCurrentChain()
	if ct==1 then
		e:SetLabel(0)
	elseif not Duel.CheckChainUniqueness() then
		e:SetLabel(2)
	elseif ct>=3 and e:GetLabel()~=2 then
		e:SetLabel(1)
	end
end
function c45023678.descon(e,tp,eg,ep,ev,re,r,rp)
	local res=e:GetLabelObject():GetLabel()
	e:GetLabelObject():SetLabel(0)
	return res==1
end
function c45023678.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c45023678.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
