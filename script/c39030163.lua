--ギャラクシーアイズ FA・フォトン・ドラゴン
function c39030163.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,8,3,c39030163.ovfilter,aux.Stringid(39030163,0))
	c:EnableReviveLimit()
	--material
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(39030163,1))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetCost(c39030163.mtcost)
	e1:SetTarget(c39030163.mttg)
	e1:SetOperation(c39030163.mtop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(39030163,2))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetCost(c39030163.descost)
	e2:SetTarget(c39030163.destg)
	e2:SetOperation(c39030163.desop)
	c:RegisterEffect(e2)
end
function c39030163.ovfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x107b) and c:IsType(TYPE_XYZ) and not c:IsCode(39030163)
end
function c39030163.mtcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c39030163.mttg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local g=e:GetHandler():GetEquipGroup()
	if chkc then return g:IsContains(chkc) and chkc:IsCanBeEffectTarget(e) end
	if chk==0 then return g:IsExists(Card.IsCanBeEffectTarget,1,nil,e) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local tg=g:FilterSelect(tp,Card.IsCanBeEffectTarget,1,2,nil,e)
	Duel.SetTargetCard(tg)
end
function c39030163.mtfilter(c,e)
	return c:IsRelateToEffect(e) and not c:IsImmuneToEffect(e)
end
function c39030163.mtop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c39030163.mtfilter,nil,e)
	if g:GetCount()>0 then
		Duel.Overlay(c,g)
	end
end
function c39030163.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c39030163.desfilter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c39030163.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and c39030163.desfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c39030163.desfilter,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c39030163.desfilter,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c39030163.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
