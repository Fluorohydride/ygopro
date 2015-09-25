--彼岸の巡礼者 ダンテ
function c18386170.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCondition(c18386170.fscon)
	e1:SetOperation(c18386170.fsop)
	c:RegisterEffect(e1)
	--special summon condition
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetValue(aux.fuslimit)
	c:RegisterEffect(e2)
	--cannot be target
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetValue(c18386170.tgval)
	c:RegisterEffect(e3)
	--draw
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_DRAW)
	e4:SetType(EFFECT_TYPE_QUICK_O)
	e4:SetCode(EVENT_FREE_CHAIN)
	e4:SetRange(LOCATION_MZONE)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetCountLimit(1)
	e4:SetCost(c18386170.drcost)
	e4:SetTarget(c18386170.drtg)
	e4:SetOperation(c18386170.drop)
	c:RegisterEffect(e4)
	--handes
	local e5=Effect.CreateEffect(c)
	e5:SetCategory(CATEGORY_HANDES)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e5:SetCode(EVENT_TO_GRAVE)
	e5:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e5:SetCondition(c18386170.hdcon)
	e5:SetTarget(c18386170.hdtg)
	e5:SetOperation(c18386170.hdop)
	c:RegisterEffect(e5)
end
function c18386170.tgval(e,re,rp)
	return rp~=e:GetHandlerPlayer() and not re:GetHandler():IsImmuneToEffect(e)
end
function c18386170.cfilter(c)
	return c:IsSetCard(0xb1) and c:IsAbleToGraveAsCost()
end
function c18386170.drcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c18386170.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c18386170.cfilter,1,1,REASON_COST,nil)
end
function c18386170.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c18386170.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c18386170.hdcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return (rp~=tp and c:IsReason(REASON_EFFECT) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_ONFIELD)) or c:IsReason(REASON_BATTLE)
end
function c18386170.hdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,1)
end
function c18386170.hdop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g:GetCount()==0 then return end
	local sg=g:RandomSelect(tp,1)
	Duel.SendtoGrave(sg,REASON_EFFECT)
end
function c18386170.fscon(e,g,gc,chkf)
	if g==nil then return true end
	if gc then
		local mg=g:Filter(Card.IsSetCard,nil,0xb1)
		mg:AddCard(gc)
		return gc:IsSetCard(0xb1) and mg:GetClassCount(Card.GetCode)>=3
	end
	local fs=false
	local mg=g:Filter(Card.IsSetCard,nil,0xb1)
	if mg:IsExists(aux.FConditionCheckF,1,nil,chkf) then fs=true end
	return mg:GetClassCount(Card.GetCode)>=3 and (fs or chkf==PLAYER_NONE)
end
function c18386170.fsop(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
	if gc then
		local sg=eg:Filter(Card.IsSetCard,gc,0xb1)
		sg:Remove(Card.IsCode,nil,gc:GetCode())
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		local g1=sg:Select(tp,1,1,nil)
		sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		local g2=sg:Select(tp,1,1,nil)
		g1:Merge(g2)
		Duel.SetFusionMaterial(g1)
		return
	end
	local sg=eg:Filter(Card.IsSetCard,nil,0xb1)
	local g1=nil
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf)
	else g1=sg:Select(tp,1,1,nil) end
	sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g2=sg:Select(tp,1,1,nil)
	sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g3=sg:Select(tp,1,1,nil)
	g1:Merge(g2)
	g1:Merge(g3)
	Duel.SetFusionMaterial(g1)
end
