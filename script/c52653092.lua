--ＳＮｏ.０ ホープ・ゼアル
function c52653092.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c52653092.xyzcon)
	e1:SetOperation(c52653092.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--cannot disable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DISABLE_SPSUMMON)
	e2:SetCondition(c52653092.ndcon)
	c:RegisterEffect(e2)
	--activate limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetOperation(c52653092.sumop)
	c:RegisterEffect(e3)
	--atk/def up
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_UPDATE_ATTACK)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetValue(c52653092.val)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e5)
	--activate limit
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(52653092,1))
	e6:SetType(EFFECT_TYPE_QUICK_O)
	e6:SetCode(EVENT_FREE_CHAIN)
	e6:SetRange(LOCATION_MZONE)
	e6:SetHintTiming(0,TIMING_DRAW_PHASE)
	e6:SetCountLimit(1)
	e6:SetCondition(c52653092.nacon)
	e6:SetCost(c52653092.nacost)
	e6:SetOperation(c52653092.naop)
	c:RegisterEffect(e6)
end
c52653092.xyz_number=0
function c52653092.cfilter(c)
	return c:IsSetCard(0x95) and c:GetType()==TYPE_SPELL and c:IsDiscardable()
end
function c52653092.ovfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x7f)
end
function c52653092.mfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsSetCard(0x48)
end
function c52653092.xyzfilter1(c,g)
	return g:IsExists(c52653092.xyzfilter2,2,c,c:GetRank())
end
function c52653092.xyzfilter2(c,rk)
	return c:GetRank()==rk
end
function c52653092.xyzcon(e,c,og)
	if c==nil then return true end
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=-ft
	if ct>=2 then return false end
	if ct<1 and Duel.IsExistingMatchingCard(aux.XyzAlterFilter,tp,LOCATION_MZONE,0,1,nil,c52653092.ovfilter,c)
		and Duel.IsExistingMatchingCard(c52653092.cfilter,tp,LOCATION_HAND,0,1,nil) then
		return true
	end
	local mg=Duel.GetMatchingGroup(c52653092.mfilter,tp,LOCATION_MZONE,0,nil)
	return mg:IsExists(c5727.xyzfilter1,1,nil,mg)
end
function c52653092.xyzop(e,tp,eg,ep,ev,re,r,rp,c,og)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=-ft
	local mg=Duel.GetMatchingGroup(c52653092.mfilter,tp,LOCATION_MZONE,0,nil)
	local b1=mg:IsExists(c52653092.xyzfilter1,1,nil,mg)
	local b2=ct<1 and Duel.IsExistingMatchingCard(aux.XyzAlterFilter,tp,LOCATION_MZONE,0,1,nil,c52653092.ovfilter,c)
		and Duel.IsExistingMatchingCard(c52653092.cfilter,tp,LOCATION_HAND,0,1,nil)
	if b2 and (not b1 or Duel.SelectYesNo(tp,aux.Stringid(52653092,0))) then
		Duel.DiscardHand(tp,c52653092.cfilter,1,1,REASON_COST+REASON_DISCARD)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g=Duel.SelectMatchingCard(tp,aux.XyzAlterFilter,tp,LOCATION_MZONE,0,1,1,nil,c52653092.ovfilter,c)
		local g2=g:GetFirst():GetOverlayGroup()
		if g2:GetCount()~=0 then
			Duel.Overlay(c,g2)
		end
		c:SetMaterial(g)
		Duel.Overlay(c,g)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g1=mg:FilterSelect(tp,c52653092.xyzfilter1,1,1,nil,mg)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g2=mg:FilterSelect(tp,c52653092.xyzfilter2,2,2,g1:GetFirst(),g1:GetFirst():GetRank())
		g1:Merge(g2)
		local sg=Group.CreateGroup()
		local tc=g1:GetFirst()
		while tc do
			sg:Merge(tc:GetOverlayGroup())
			tc=g1:GetNext()
		end
		Duel.SendtoGrave(sg,REASON_RULE)
		c:SetMaterial(g1)
		Duel.Overlay(c,g1)
	end
end
function c52653092.ndcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_XYZ
end
function c52653092.sumop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetSummonType()~=SUMMON_TYPE_XYZ then return end
	Duel.SetChainLimitTillChainEnd(c52653092.chainlm)
end
function c52653092.chainlm(e,rp,tp)
	return tp==rp
end
function c52653092.val(e,c)
	return c:GetOverlayCount()*1000
end
function c52653092.nacon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c52653092.nacost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c52653092.naop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	e1:SetValue(aux.TRUE)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
