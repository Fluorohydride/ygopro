--SNo.0 ホープ・ゼアル
function c52653092.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetCondition(c52653092.xyzcon)
	e1:SetOperation(c52653092.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--cannot disable spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DISABLE_SPSUMMON)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetCondition(c52653092.effcon)
	c:RegisterEffect(e2)
	--summon success
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetCondition(c52653092.effcon2)
	e3:SetOperation(c52653092.spsumsuc)
	c:RegisterEffect(e3)
	--atk & def
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_UPDATE_ATTACK)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetValue(c52653092.atkval)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e5)
	--activate limit
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_QUICK_O)
	e6:SetCode(EVENT_FREE_CHAIN)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCountLimit(1)
	e6:SetCondition(c52653092.actcon)
	e6:SetCost(c52653092.actcost)
	e6:SetOperation(c52653092.actop)
	c:RegisterEffect(e6)
end
c52653092.xyz_number=0
function c52653092.effcon(e)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_XYZ
end
function c52653092.effcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_XYZ
end
function c52653092.spsumsuc(e,tp,eg,ep,ev,re,r,rp)
	Duel.SetChainLimitTillChainEnd(c52653092.chlimit)
end
function c52653092.chlimit(e,ep,tp)
	return tp==ep
end
function c52653092.atkval(e,c)
	return e:GetHandler():GetOverlayCount()*1000
end
function c52653092.actcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c52653092.actcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c52653092.actop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetTargetRange(0,1)
	e1:SetValue(aux.TRUE)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c52653092.mfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsSetCard(0x48)
end
function c52653092.xyzfilter1(c,g)
	return g:IsExists(c52653092.xyzfilter2,1,c,g,c)
end
function c52653092.xyzfilter2(c,g,oc)
	local mg=g:Clone()
	mg:RemoveCard(oc)
	return c:GetRank()==oc:GetRank() and mg:IsExists(c52653092.xyzfilter3,1,c,c:GetRank())
end
function c52653092.xyzfilter3(c,rk)
	return c:GetRank()==rk
end
function c52653092.ovfilter(c,xyzc)
	return c:IsFaceup() and c:IsSetCard(0x7f) and c:IsCanBeXyzMaterial(xyzc)
end
function c52653092.disfilter(c)
	return c:IsSetCard(0x95) and c:GetType()==TYPE_SPELL and c:IsDiscardable()
end
function c52653092.xyzcon(e,c,og)
	if c==nil then return true end
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=-ft
	if 3<=ct then return false end
	if ct<1 and Duel.IsExistingMatchingCard(c52653092.ovfilter,tp,LOCATION_MZONE,0,1,nil,c)
		and Duel.IsExistingMatchingCard(c52653092.disfilter,tp,LOCATION_HAND,0,1,nil) then
		return true
	end
	local mg=Duel.GetMatchingGroup(c52653092.mfilter,tp,LOCATION_MZONE,0,nil)
	return mg:IsExists(c52653092.xyzfilter1,1,nil,mg)
end
function c52653092.xyzop(e,tp,eg,ep,ev,re,r,rp,c,og)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=-ft
	local mg=Duel.GetMatchingGroup(c52653092.mfilter,tp,LOCATION_MZONE,0,nil)
	local b1=mg:IsExists(c52653092.xyzfilter1,1,nil,mg)
	local b2=ct<1 and Duel.IsExistingMatchingCard(c52653092.ovfilter,tp,LOCATION_MZONE,0,1,nil,c)
		and Duel.IsExistingMatchingCard(c52653092.disfilter,tp,LOCATION_HAND,0,1,nil)
	if b2 and (not b1 or Duel.SelectYesNo(tp,aux.Stringid(52653092,0))) then
		Duel.DiscardHand(tp,c52653092.disfilter,1,1,REASON_COST,nil)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local ovg=Duel.SelectMatchingCard(tp,c52653092.ovfilter,tp,LOCATION_MZONE,0,1,1,nil,c)
		local ovg2=ovg:GetFirst():GetOverlayGroup()
		if ovg2:GetCount()~=0 then
			Duel.Overlay(c,ovg2)
		end
		c:SetMaterial(ovg)
		Duel.Overlay(c,ovg)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g1=mg:FilterSelect(tp,c52653092.xyzfilter1,1,1,nil,mg)
		local tc1=g1:GetFirst()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g2=mg:FilterSelect(tp,c52653092.xyzfilter2,1,1,tc1,mg,tc1)
		local tc2=g2:GetFirst()
		mg:RemoveCard(tc1)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g3=mg:FilterSelect(tp,c52653092.xyzfilter3,1,1,tc2,tc2:GetRank())
		local tc3=g3:GetFirst()
		g1:Merge(g2)
		g1:Merge(g3)
		local sg1=tc1:GetOverlayGroup()
		local sg2=tc2:GetOverlayGroup()
		local sg3=tc3:GetOverlayGroup()
		sg1:Merge(sg2)
		sg1:Merge(sg3)
		Duel.SendtoGrave(sg1,REASON_RULE)
		c:SetMaterial(g1)
		Duel.Overlay(c,g1)
	end
end
