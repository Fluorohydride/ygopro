--渇きの風
function c28265983.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_RECOVER,0)
	e1:SetTarget(c28265983.target)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(28265983,1))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_RECOVER)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,28265983)
	e2:SetCondition(c28265983.descon1)
	e2:SetTarget(c28265983.destg1)
	e2:SetOperation(c28265983.desop1)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(28265983,2))
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1,28265984)
	e3:SetCondition(c28265983.descon2)
	e3:SetCost(c28265983.descost2)
	e3:SetTarget(c28265983.destg2)
	e3:SetOperation(c28265983.desop2)
	c:RegisterEffect(e3)
end
function c28265983.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return c28265983.destg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc) end
	if chk==0 then return true end
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_RECOVER,true)
	local b1=res and c28265983.descon1(e,tp,teg,tep,tev,tre,tr,trp) and c28265983.destg1(e,tp,teg,tep,tev,tre,tr,trp,0)
	local b2=c28265983.descon2(e,tp,eg,ep,ev,re,r,rp) and c28265983.descost2(e,tp,eg,ep,ev,re,r,rp,0) and c28265983.destg2(e,tp,eg,ep,ev,re,r,rp,0)
	if (b1 or b2) and Duel.SelectYesNo(tp,94) then
		local opt=0
		if b1 and b2 then
			opt=Duel.SelectOption(tp,aux.Stringid(28265983,1),aux.Stringid(28265983,2))
		elseif b1 then
			opt=Duel.SelectOption(tp,aux.Stringid(28265983,1))
		else
			opt=Duel.SelectOption(tp,aux.Stringid(28265983,2))+1
		end
		if opt==0 then
			e:SetCategory(CATEGORY_DESTROY)
			e:SetProperty(EFFECT_FLAG_CARD_TARGET)
			e:SetOperation(c28265983.desop1)
			c28265983.destg1(e,tp,teg,tep,tev,tre,tr,trp,1)
		else
			e:SetCategory(CATEGORY_DESTROY)
			e:SetProperty(0)
			e:SetOperation(c28265983.desop2)
			c28265983.descost2(e,tp,eg,ep,ev,re,r,rp,1)
			c28265983.destg2(e,tp,eg,ep,ev,re,r,rp,1)
		end
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetOperation(nil)
	end
end
function c28265983.descon1(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp
end
function c28265983.desfilter1(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c28265983.destg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c28265983.desfilter1(chkc) end
	if chk==0 then return Duel.GetFlagEffect(tp,28265983)==0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c28265983.desfilter1,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.RegisterFlagEffect(tp,28265983,RESET_PHASE+RESET_END,0,1)
end
function c28265983.desop1(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c28265983.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xc9)
end
function c28265983.descon2(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c28265983.cfilter,tp,LOCATION_MZONE,0,1,nil) and Duel.GetLP(tp)-Duel.GetLP(1-tp)>=3000
end
function c28265983.descost2(e,tp,eg,ep,ev,re,r,rp,chk)
	local lp=Duel.GetLP(tp)-Duel.GetLP(1-tp)
	if chk==0 then return Duel.CheckLPCost(tp,lp) and Duel.GetFlagEffect(tp,28265984)==0 end
	Duel.PayLPCost(tp,lp)
	e:SetLabel(lp)
	Duel.RegisterFlagEffect(tp,28265984,RESET_PHASE+RESET_END,0,1)
end
function c28265983.desfilter2(c,num)
	return c:IsFaceup() and c:IsAttackBelow(num) and c:IsDestructable()
end
function c28265983.destg2(e,tp,eg,ep,ev,re,r,rp,chk)
	local lp=Duel.GetLP(tp)-Duel.GetLP(1-tp)
	if chk==0 then return Duel.IsExistingMatchingCard(c28265983.desfilter2,tp,0,LOCATION_MZONE,1,nil,lp) end
	local g=Duel.GetMatchingGroup(c28265983.desfilter2,tp,0,LOCATION_MZONE,nil,e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c28265983.desop2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local num=e:GetLabel()
	local g=Duel.GetMatchingGroup(c28265983.desfilter2,tp,0,LOCATION_MZONE,nil,num)
	if g:GetCount()==0 then return end
	local dg=Group.CreateGroup()
	repeat
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local tg=g:FilterSelect(tp,c28265983.desfilter2,1,1,nil,num)
		local tc=tg:GetFirst()
		num=num-tc:GetAttack()
		g:RemoveCard(tc)
		dg:AddCard(tc)
	until not g:IsExists(c28265983.desfilter2,1,nil,num) or not Duel.SelectYesNo(tp,aux.Stringid(28265983,3))
	Duel.Destroy(dg,REASON_EFFECT)
end
