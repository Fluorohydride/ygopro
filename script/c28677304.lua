--E·HERO ブラック·ネオス
function c28677304.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode2(c,89943723,43237273,false,false)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c28677304.splimit)
	c:RegisterEffect(e1)
	--special summon rule
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_EXTRA)
	e2:SetCondition(c28677304.spcon)
	e2:SetOperation(c28677304.spop)
	c:RegisterEffect(e2)
	--return
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(28677304,0))
	e3:SetCategory(CATEGORY_TODECK)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c28677304.retcon1)
	e3:SetTarget(c28677304.rettg)
	e3:SetOperation(c28677304.retop)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetProperty(0)
	e4:SetCondition(c28677304.retcon2)
	c:RegisterEffect(e4)
	--disable
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(28677304,1))
	e5:SetCategory(CATEGORY_DISABLE)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetRange(LOCATION_MZONE)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetCondition(c28677304.discon)
	e5:SetTarget(c28677304.distg)
	e5:SetOperation(c28677304.disop)
	c:RegisterEffect(e5)
end
function c28677304.splimit(e,se,sp,st)
	return not e:GetHandler():IsLocation(LOCATION_EXTRA)
end
function c28677304.spfilter(c,code)
	if c:GetCode()~=code then return false end
	return c:IsAbleToDeckAsCost() or c:IsAbleToExtraAsCost()
end
function c28677304.spcon(e,c)
	if c==nil then return true end 
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<-1 then return false end
	local g1=Duel.GetMatchingGroup(c28677304.spfilter,tp,LOCATION_ONFIELD,0,nil,89943723)
	local g2=Duel.GetMatchingGroup(c28677304.spfilter,tp,LOCATION_ONFIELD,0,nil,43237273)
	if g1:GetCount()==0 or g2:GetCount()==0 then return false end
	if ft>0 then return true end
	local f1=g1:FilterCount(Card.IsLocation,nil,LOCATION_MZONE)
	local f2=g2:FilterCount(Card.IsLocation,nil,LOCATION_MZONE)
	if ft==-1 then return f1>0 and f2>0
	else return f1>0 or f2>0 end
end
function c28677304.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local g1=Duel.GetMatchingGroup(c28677304.spfilter,tp,LOCATION_ONFIELD,0,nil,89943723)
	local g2=Duel.GetMatchingGroup(c28677304.spfilter,tp,LOCATION_ONFIELD,0,nil,43237273)
	g1:Merge(g2)
	local g=Group.CreateGroup()
	local tc=nil
	for i=1,2 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		if ft<=0 then
			tc=g1:FilterSelect(tp,Card.IsLocation,1,1,nil,LOCATION_MZONE):GetFirst()
		else
			tc=g1:Select(tp,1,1,nil):GetFirst()
		end
		g:AddCard(tc)
		g1:Remove(Card.IsCode,nil,tc:GetCode())
		ft=ft+1
	end
	local cg=g:Filter(Card.IsFacedown,nil)
	if cg:GetCount()>0 then
		Duel.ConfirmCards(1-tp,cg)
	end
	Duel.SendtoDeck(g,nil,2,REASON_COST)
end
function c28677304.retcon1(e,tp,eg,ep,ev,re,r,rp,chk)
	return not e:GetHandler():IsHasEffect(42015635)
end
function c28677304.retcon2(e,tp,eg,ep,ev,re,r,rp,chk)
	return e:GetHandler():IsHasEffect(42015635)
end
function c28677304.rettg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToExtra() end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,e:GetHandler(),1,0,0)
end
function c28677304.retop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) or e:GetHandler():IsFacedown() then return end
	Duel.SendtoDeck(e:GetHandler(),nil,2,REASON_EFFECT)
end
function c28677304.discon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetCardTargetCount()==0
end
function c28677304.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT)
end
function c28677304.distg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c28677304.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c28677304.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c28677304.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,g,1,0,0)
end
function c28677304.disop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c28677304.rcon)
		tc:RegisterEffect(e1,true)
	end
end
function c28677304.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
