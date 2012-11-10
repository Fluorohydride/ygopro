--セフェルの魔導書
function c56981417.initial_effect(c)
	--copy spell
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c56981417.cost)
	e1:SetTarget(c56981417.target)
	e1:SetOperation(c56981417.operation)
	c:RegisterEffect(e1)
end
function c56981417.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,56981417)==0 end
	Duel.RegisterFlagEffect(tp,56981417,RESET_EVENT+0x1fe0000,0,1)
end
function c56981417.filter(c)
	return c:IsSetCard(0x106e) and c:GetType()==TYPE_SPELL and c:CheckActivateEffect(true,true,false)~=nil
end
function c56981417.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c56981417.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c56981417.filter,tp,LOCATION_GRAVE,0,1,nil) end
	e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e:SetCategory(0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c56981417.filter,tp,LOCATION_GRAVE,0,1,1,nil)
end
function c56981417.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) or tc:CheckActivateEffect(true,true,false)==nil then return end
	local tpe=tc:GetType()
	local te=tc:GetActivateEffect()
	local tg=te:GetTarget()
	local op=te:GetOperation()
	e:SetCategory(te:GetCategory())
	e:SetProperty(te:GetProperty())
	Duel.ClearTargetCard()
	if tg then tg(e,tp,eg,ep,ev,re,r,rp,1) end
	Duel.BreakEffect()
	if op then op(e,tp,eg,ep,ev,re,r,rp) end
end
