Auxiliary={}
aux=Auxiliary

function Auxiliary.Stringid(code,id)
	return code*16+id
end
function Auxiliary.Next(g)
	local first=true
	return	function()
				if first then first=false return g:GetFirst()
				else return g:GetNext() end
			end
end
function Auxiliary.NULL()
end
function Auxiliary.TRUE()
	return true
end
function Auxiliary.FALSE()
	return false
end
function Auxiliary.AND(f1,f2)
	return 	function(a,b,c)
				return f1(a,b,c) and f2(a,b,c)
			end
end
function Auxiliary.OR(f1,f2)
	return 	function(a,b,c)
				return f1(a,b,c) or f2(a,b,c)
			end
end
function Auxiliary.NOT(f)
	return 	function(a,b,c)
				return not f(a,b,c)
			end
end
function Auxiliary.IsDualState(effect)
	local c=effect:GetHandler()
	return not c:IsDisabled() and c:IsDualState()
end
function Auxiliary.IsNotDualState(effect)
	local c=effect:GetHandle()
	return c:IsDisabled() or not c:IsDualState()
end
function Auxiliary.DualNormalCondition(effect)
	local c=effect:GetHandler()
	return c:IsFaceup() and not c:IsDualState()
end
function Auxiliary.BeginPuzzle(effect)
	local e1=Effect.GlobalEffect()
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TURN_END)
	e1:SetCountLimit(1)
	e1:SetOperation(Auxiliary.PuzzleOp)
	Duel.RegisterEffect(e1,0)
	local e2=Effect.GlobalEffect()
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EFFECT_SKIP_DP)
	e2:SetTargetRange(1,0)
	Duel.RegisterEffect(e2,0)
	local e3=Effect.GlobalEffect()
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetCode(EFFECT_SKIP_SP)
	e3:SetTargetRange(1,0)
	Duel.RegisterEffect(e3,0)
end
function Auxiliary.PuzzleOp(e,tp)
	Duel.SetLP(0,0)
end
function Auxiliary.EnableDualAttribute(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DUAL_SUMMONABLE)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CHANGE_TYPE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE+LOCATION_GRAVE)
	e2:SetCondition(aux.DualNormalCondition)
	e2:SetValue(TYPE_NORMAL+TYPE_DUAL+TYPE_MONSTER)
	c:RegisterEffect(e2)
end
function Auxiliary.TargetEqualFunction(f,value,a,b,c)
	return 	function(effect,target)
				return f(target,a,b,c)==value 
			end
end
function Auxiliary.TargetBoolFunction(f,a,b,c)
	return 	function(effect,target)
				return f(target,a,b,c)
			end
end
function Auxiliary.FilterEqualFunction(f,value,a,b,c)
	return 	function(target)
				return f(target,a,b,c)==value 
			end
end
function Auxiliary.FilterBoolFunction(f,a,b,c)
	return 	function(target)
				return f(target,a,b,c)
			end
end
function Auxiliary.NonTuner(f,a,b,c)
	return	function(target)
				return target:IsNotTuner() and (not f or f(target,a,b,c))
			end
end
function Auxiliary.AddSynchroProcedure(c,f1,f2,ct)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(Auxiliary.SynCondition(f1,f2,ct,99))
	e1:SetOperation(Auxiliary.SynOperation(f1,f2,ct,99))
	e1:SetValue(SUMMON_TYPE_SYNCHRO)
	c:RegisterEffect(e1)
end
function Auxiliary.SynCondition(f1,f2,minct,maxc)
	return	function(e,c,smat,mg)
				if c==nil then return true end
				local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
				local ct=-ft
				local minc=minct
				if minc<ct then minc=ct end
				if maxc<minc then return false end
				if smat and smat:IsType(TYPE_TUNER) and (not f1 or f1(smat)) then
					return Duel.CheckTunerMaterial(c,smat,f1,f2,minc,maxc,mg) end
				return Duel.CheckSynchroMaterial(c,f1,f2,minc,maxc,smat,mg)
			end
end
function Auxiliary.SynOperation(f1,f2,minct,maxc)
	return	function(e,tp,eg,ep,ev,re,r,rp,c,smat,mg)
				local g=nil
				local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
				local ct=-ft
				local minc=minct
				if minc<ct then minc=ct end
				if smat and smat:IsType(TYPE_TUNER) and (not f1 or f1(smat)) then
					g=Duel.SelectTunerMaterial(c:GetControler(),c,smat,f1,f2,minc,maxc,mg)
				else
					g=Duel.SelectSynchroMaterial(c:GetControler(),c,f1,f2,minc,maxc,smat,mg)
				end
				c:SetMaterial(g)
				Duel.SendtoGrave(g,REASON_MATERIAL+REASON_SYNCHRO)
			end
end
function Auxiliary.AddSynchroProcedure2(c,f1,f2)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(Auxiliary.SynCondition(f1,f2,1,1))
	e1:SetOperation(Auxiliary.SynOperation(f1,f2,1,1))
	e1:SetValue(SUMMON_TYPE_SYNCHRO)
	c:RegisterEffect(e1)
end
function Auxiliary.AddXyzProcedure(c,f,lv,ct,alterf,desc,maxct,op)
	if c.xyz_filter==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.xyz_filter=f
		mt.xyz_count=ct
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	if not maxct then maxct=ct end
	if alterf then
		e1:SetCondition(Auxiliary.XyzCondition2(f,lv,ct,maxct,alterf,desc,op))
		e1:SetOperation(Auxiliary.XyzOperation2(f,lv,ct,maxct,alterf,desc,op))
	else
		e1:SetCondition(Auxiliary.XyzCondition(f,lv,ct,maxct))
		e1:SetOperation(Auxiliary.XyzOperation(f,lv,ct,maxct))
	end
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
end
function Auxiliary.XyzCondition(f,lv,minc,maxc)
	--og: use special material
	return	function(e,c,og)
				if c==nil then return true end
				local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
				local ct=-ft
				if minc<=ct then return false end
				return Duel.CheckXyzMaterial(c,f,lv,minc,maxc,og)
			end
end
function Auxiliary.XyzOperation(f,lv,minc,maxc)
	return	function(e,tp,eg,ep,ev,re,r,rp,c,og)
				if og then
					c:SetMaterial(og)
					Duel.Overlay(c,og)
				else
					local mg=Duel.SelectXyzMaterial(tp,c,f,lv,minc,maxc)
					c:SetMaterial(mg)
					Duel.Overlay(c,mg)
				end
			end
end
function Auxiliary.XyzCondition2(f,lv,minc,maxc,alterf,desc,op)
	return	function(e,c,og)
				if c==nil then return true end
				local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
				local ct=-ft
				if minc<=ct then return false end
				if ct<1 and Duel.IsExistingMatchingCard(alterf,c:GetControler(),LOCATION_MZONE,0,1,nil)
					and (not op or op(e,c:GetControler(),0)) then
					return true
				end
				return Duel.CheckXyzMaterial(c,f,lv,minc,maxc,og)
			end
end
function Auxiliary.XyzOperation2(f,lv,minc,maxc,alterf,desc,op)
	return	function(e,tp,eg,ep,ev,re,r,rp,c,og)
				if og then
					c:SetMaterial(og)
					Duel.Overlay(c,og)
				else
					local ft=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)
					local ct=-ft
					local b1=Duel.CheckXyzMaterial(c,f,lv,minc,maxc,og)
					local b2=ct<1 and Duel.IsExistingMatchingCard(alterf,tp,LOCATION_MZONE,0,1,nil)
						and (not op or op(e,tp,0))
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
					if b2 and (not b1 or Duel.SelectYesNo(tp,desc)) then
						if op then op(e,tp,1) end
						local mg=Duel.SelectMatchingCard(tp,alterf,tp,LOCATION_MZONE,0,1,1,nil)
						local mg2=mg:GetFirst():GetOverlayGroup()
						if mg2:GetCount()~=0 then
							Duel.Overlay(c,mg2)
						end
						c:SetMaterial(mg)
						Duel.Overlay(c,mg)
					else
						local mg=Duel.SelectXyzMaterial(tp,c,f,lv,minc,maxc)
						c:SetMaterial(mg)
						Duel.Overlay(c,mg)
					end
				end
			end
end
function Auxiliary.FConditionCheckF(c,chkf)
	return c:IsOnField() and c:IsControler(chkf)
end
function Auxiliary.AddFusionProcCode2(c,code1,code2,sub,insf)
	if c.material_count==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.material_count=2
		mt.material={code1,code2}
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode2(code1,code2,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode2(code1,code2,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter21(c,code1,code2)
	local code=c:GetCode()
	return code==code1 or code==code2
end
function Auxiliary.FConditionFilter22(c,code1,code2,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode2(code1,code2,sub,insf)
	--g:Material group(nil for Instant Fusion)
	--gc:Material already used
	--chkf: check field, default:PLAYER_NONE
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return g:IsExists(Auxiliary.FConditionFilter21,1,nil,code1,code2) end
				local b1=0 local b2=0 local bs=0
				local f1=false local f2=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+bs~=0) or (b2~=0 and f2 and b1+bs~=0) or (bs~=0 and fs and b1+b2~=0)
					else return (b1~=0 and f1 and b2==1) or (b2~=0 and f2 and b1==1) end
				else
					if sub then return b1+b2+bs>1
					else return b1+b2==2 end
				end
			end
end
function Auxiliary.FOperationCode2(code1,code2,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,Auxiliary.FConditionFilter21,1,1,nil,code1,code2)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter22,nil,code1,code2,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCode3(c,code1,code2,code3,sub,insf)
	if c.material_count==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.material_count=3
		mt.material={code1,code2,code3}
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode3(code1,code2,code3,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode3(code1,code2,code3,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter31(c,code1,code2,code3)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3
end
function Auxiliary.FConditionFilter32(c,code1,code2,code3,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode3(code1,code2,code3,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then
					local b1=0 local b2=0 local b3=0
					local tc=g:GetFirst()
					while tc do
						local code=tc:GetCode()
						if code==code1 then b1=1
						elseif code==code2 then b2=1
						elseif code==code3 then b3=1
						end
						tc=g:GetNext()
					end
					return b1+b2+b3>1
				end
				local b1=0 local b2=0 local b3=0 local bs=0
				local f1=false local f2=false local f3=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif code==code3 then b3=1 if Auxiliary.FConditionCheckF(tc,chkf) then f3=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+b3+bs>1) or (b2~=0 and f2 and b1+b3+bs>1)
						or (b3~=0 and f3 and b1+b2+bs>1) or (bs~=0 and fs and b1+b2+b3>1)
					else return (b1~=0 and f1 and b2+b3==2) or (b2~=0 and f2 and b1+b3==2)
						or (b3~=0 and f3 and b1+b2==2) end
				else
					if sub then return b1+b2+b3+bs>2
					else return b1+b2+b3==3 end
				end
			end
end
function Auxiliary.FOperationCode3(code1,code2,code3,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=eg:Filter(Auxiliary.FConditionFilter31,nil,code1,code2,code3)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					g1:Merge(g2)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter32,nil,code1,code2,code3,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				if g2:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g3=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				g1:Merge(g3)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCode4(c,code1,code2,code3,code4,sub,insf)
	if c.material_count==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.material_count=4
		mt.material={code1,code2,code3,code4}
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode4(code1,code2,code3,code4,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode4(code1,code2,code3,code4,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter41(c,code1,code2,code3,code4)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or code==code4
end
function Auxiliary.FConditionFilter42(c,code1,code2,code3,code4,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or code==code4 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode4(code1,code2,code3,code4,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then
					local b1=0 local b2=0 local b3=0 local b4=0
					local tc=g:GetFirst()
					while tc do
						local code=tc:GetCode()
						if code==code1 then b1=1
						elseif code==code2 then b2=1
						elseif code==code3 then b3=1
						elseif code==code4 then b4=1
						end
						tc=g:GetNext()
					end
					return b1+b2+b3+b4>2
				end
				local b1=0 local b2=0 local b3=0 local b4=0 local bs=0
				local f1=false local f2=false local f3=false local f4=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif code==code3 then b3=1 if Auxiliary.FConditionCheckF(tc,chkf) then f3=true end
					elseif code==code4 then b4=1 if Auxiliary.FConditionCheckF(tc,chkf) then f4=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+b3+b4+bs>2) or (b2~=0 and f2 and b1+b3+b4+bs>2)
						or (b3~=0 and f3 and b1+b2+b4+bs>2) or (b4~=0 and f4 and b1+b2+b3+bs>2)
						or (bs~=0 and fs and b1+b2+b3+b4>2)
					else return (b1~=0 and f1 and b2+b3+b4==3) or (b2~=0 and f2 and b1+b3+b4==3)
						or (b3~=0 and f3 and b1+b2+b4==3) or (b4~=0 and f4 and b1+b2+b3==3) end
				else
					if sub then return b1+b2+b3+b4+bs>3
					else return b1+b2+b3+b4==4 end
				end
			end
end
function Auxiliary.FOperationCode4(code1,code2,code3,code4,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=eg:Filter(Auxiliary.FConditionFilter41,nil,code1,code2,code3,code4)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g3=sg:Select(tp,1,1,nil)
					g1:Merge(g2)
					g1:Merge(g3)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter42,nil,code1,code2,code3,code4,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				if g2:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g3=sg:Select(tp,1,1,nil)
				if g3:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g3:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g4=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				g1:Merge(g3)
				g1:Merge(g4)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCodeFun(c,code1,f,cc,sub,insf)
	if c.material_count==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.material_count=1
		mt.material={code1}
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCodeFun(code1,f,cc,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCodeFun(code1,f,cc,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterCF(c,g2,cc)
	return g2:IsExists(aux.TRUE,cc,c)
end
function Auxiliary.FConditionCodeFun(code,f,cc,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return (gc:IsCode(code) or (sub and gc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)))
					and g:IsExists(f,cc,nil) end
				local g1=Group.CreateGroup() local g2=Group.CreateGroup() local fs=false
				local tc=g:GetFirst()
				while tc do
					if tc:IsCode(code) or (sub and tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
						then g1:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					if f(tc) then g2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					return fs and g1:IsExists(Auxiliary.FConditionFilterCF,1,nil,g2,cc)
				else return g1:IsExists(Auxiliary.FConditionFilterCF,1,nil,g2,cc) end
			end
end
function Auxiliary.FOperationCodeFun(code,f,cc,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,f,cc,cc,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg1=Group.CreateGroup() local sg2=Group.CreateGroup() local fs=false
				local tc=eg:GetFirst()
				while tc do
					if tc:IsCode(code) or (sub and tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) then sg1:AddCard(tc) end
					if f(tc) then sg2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=eg:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sg2:GetCount()==cc then
						sg1:Sub(sg2)
					end
					local g1=nil
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					if fs then g1=sg1:Select(tp,1,1,nil)
					else g1=sg1:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf) end
					local tc1=g1:GetFirst()
					sg2:RemoveCard(tc1)
					if Auxiliary.FConditionCheckF(tc1,chkf) or sg2:GetCount()==cc then
						Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
						local g2=sg2:Select(tp,cc,cc,tc1)
						g1:Merge(g2)
					else
						Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
						local g2=sg2:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,tc1,chkf)
						g1:Merge(g2)
						if cc>1 then
							sg2:RemoveCard(g2:GetFirst())
							Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
							g2=sg2:Select(tp,cc-1,cc-1,tc1)
							g1:Merge(g2)
						end
					end
					Duel.SetFusionMaterial(g1)
				else
					if sg2:GetCount()==cc then
						sg1:Sub(sg2)
					end
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg1:Select(tp,1,1,nil)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					g1:Merge(sg2:Select(tp,cc,cc,g1:GetFirst()))
					Duel.SetFusionMaterial(g1)
				end
			end
end
function Auxiliary.AddFusionProcFun2(c,f1,f2,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionFun2(f1,f2,insf))
	e1:SetOperation(Auxiliary.FOperationFun2(f1,f2,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterF2(c,g2)
	return g2:IsExists(aux.TRUE,1,c)
end
function Auxiliary.FConditionFilterF2c(c,f1,f2)
	return f1(c) or f2(c)
end
function Auxiliary.FConditionFilterF2r(c,f1,f2)
	return f1(c) and not f2(c)
end
function Auxiliary.FConditionFun2(f1,f2,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return (f1(gc) and g:IsExists(f2,1,gc))
					or (f2(gc) and g:IsExists(f1,1,gc)) end
				local g1=Group.CreateGroup() local g2=Group.CreateGroup() local fs=false
				local tc=g:GetFirst()
				while tc do
					if f1(tc) then g1:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					if f2(tc) then g2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					return fs and g1:IsExists(Auxiliary.FConditionFilterF2,1,nil,g2)
				else return g1:IsExists(Auxiliary.FConditionFilterF2,1,nil,g2) end
			end
end
function Auxiliary.FOperationFun2(f1,f2,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=Group.CreateGroup()
					if f1(gc) then sg:Merge(eg:Filter(f2,gc)) end
					if f2(gc) then sg:Merge(eg:Filter(f1,gc)) end
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilterF2c,nil,f1,f2)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then
					g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				local tc1=g1:GetFirst()
				sg:RemoveCard(tc1)
				local b1=f1(tc1)
				local b2=f2(tc1)
				if b1 and not b2 then sg:Remove(Auxiliary.FConditionFilterF2r,nil,f1,f2) end
				if b2 and not b1 then sg:Remove(Auxiliary.FConditionFilterF2r,nil,f2,f1) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCodeRep(c,code1,cc,sub,insf)
	if c.material_count==nil then
		local code=c:GetOriginalCode()
		local mt=_G["c" .. code]
		mt.material_count=1
		mt.material={code1}
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCodeRep(code1,cc,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCodeRep(code1,cc,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterCR(c,code,sub)
	return c:IsCode(code) or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCodeRep(code,cc,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return gc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) and g:IsExists(Card.IsCode,cc-1,nil,code) end
				local g1=g:Filter(Card.IsCode,nil,code)
				if not sub then
					if chkf~=PLAYER_NONE then return g1:GetCount()>=cc and g1:FilterCount(Card.IsOnField,nil)~=0
					else return g1:GetCount()>=cc end
				end
				local g2=g:Filter(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				if chkf~=PLAYER_NONE then
					return (g1:FilterCount(Card.IsOnField,nil)~=0 or g2:FilterCount(Card.IsOnField,nil)~=0)
						and g1:GetCount()>=cc-1 and g1:GetCount()+g2:GetCount()>=cc
				else return g1:GetCount()>=cc-1 and g1:GetCount()+g2:GetCount()>=cc end
			end
end
function Auxiliary.FOperationCodeRep(code,cc,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,Card.IsCode,cc-1,cc-1,nil,code)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilterCR,nil,code,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				local tc1=g1:GetFirst()
				for i=1,cc-1 do
					if tc1:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
					else sg:RemoveCard(tc1) end
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					tc1=g2:GetFirst()
					g1:Merge(g2)
				end
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcFunRep(c,f,cc,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionFunRep(f,cc,insf))
	e1:SetOperation(Auxiliary.FOperationFunRep(f,cc,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFunRep(f,cc,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return f(gc) and g:IsExists(f,cc-1,nil) end
				local g1=g:Filter(f,nil)
				if chkf~=PLAYER_NONE then
					return g1:FilterCount(Card.IsOnField,nil)~=0 and g1:GetCount()>=cc
				else return g1:GetCount()>=cc end
			end
end
function Auxiliary.FOperationFunRep(f,cc,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,f,cc-1,cc-1,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(f,nil)
				if chkf==PLAYER_NONE or sg:GetCount()>=cc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,cc,cc,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				local tc1=g1:GetFirst()
				sg:RemoveCard(tc1)
				if cc>1 then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,cc-1,cc-1,nil)
					g1:Merge(g2)
				end
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddRitualProcGreater(c,filter)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(Auxiliary.RPGTarget(filter))
	e1:SetOperation(Auxiliary.RPGOperation(filter))
	c:RegisterEffect(e1)
end
function Auxiliary.RPGFilter(c,filter,e,tp,m)
	if (filter and not filter(c)) or not c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_RITUAL,tp,true,false) then return false end
	local result=false
	if m:IsContains(c) then
		m:RemoveCard(c)
		result=m:CheckWithSumGreater(Card.GetRitualLevel,c:GetLevel(),c)
		m:AddCard(c)
	else
		result=m:CheckWithSumGreater(Card.GetRitualLevel,c:GetLevel(),c)
	end
	return result
end
function Auxiliary.RPGTarget(filter)
	return	function(e,tp,eg,ep,ev,re,r,rp,chk)
				if chk==0 then
					local mg=Duel.GetRitualMaterial(tp)
					return Duel.IsExistingMatchingCard(Auxiliary.RPGFilter,tp,LOCATION_HAND,0,1,nil,filter,e,tp,mg)
				end
				Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
			end
end
function Auxiliary.RPGOperation(filter)
	return	function(e,tp,eg,ep,ev,re,r,rp)
				local mg=Duel.GetRitualMaterial(tp)
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
				local tg=Duel.SelectMatchingCard(tp,Auxiliary.RPGFilter,tp,LOCATION_HAND,0,1,1,nil,filter,e,tp,mg)
				if tg:GetCount()>0 then
					local tc=tg:GetFirst()
					mg:RemoveCard(tc)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
					local mat=mg:SelectWithSumGreater(tp,Card.GetRitualLevel,tc:GetLevel(),tc)
					tc:SetMaterial(mat)
					Duel.ReleaseRitualMaterial(mat)
					Duel.BreakEffect()
					Duel.SpecialSummon(tc,SUMMON_TYPE_RITUAL,tp,tp,true,false,POS_FACEUP)
					tc:CompleteProcedure()
				end
			end
end
function Auxiliary.AddRitualProcEqual(c,filter)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(Auxiliary.RPETarget(filter))
	e1:SetOperation(Auxiliary.RPEOperation(filter))
	c:RegisterEffect(e1)
end
function Auxiliary.RPEFilter(c,filter,e,tp,m)
	if (filter and not filter(c)) or not c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_RITUAL,tp,true,false) then return false end
	local result=false
	if m:IsContains(c) then
		m:RemoveCard(c)
		result=m:CheckWithSumEqual(Card.GetRitualLevel,c:GetLevel(),1,99,c)
		m:AddCard(c)
	else
		result=m:CheckWithSumEqual(Card.GetRitualLevel,c:GetLevel(),1,99,c)
	end
	return result
end
function Auxiliary.RPETarget(filter)
	return	function(e,tp,eg,ep,ev,re,r,rp,chk)
				if chk==0 then
					local mg=Duel.GetRitualMaterial(tp)
					return Duel.IsExistingMatchingCard(Auxiliary.RPEFilter,tp,LOCATION_HAND,0,1,nil,filter,e,tp,mg)
				end
				Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
			end
end
function Auxiliary.RPEOperation(filter)
	return	function(e,tp,eg,ep,ev,re,r,rp)
				local mg=Duel.GetRitualMaterial(tp)
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
				local tg=Duel.SelectMatchingCard(tp,Auxiliary.RPEFilter,tp,LOCATION_HAND,0,1,1,nil,filter,e,tp,mg)
				if tg:GetCount()>0 then
					local tc=tg:GetFirst()
					mg:RemoveCard(tc)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
					local mat=mg:SelectWithSumEqual(tp,Card.GetRitualLevel,tc:GetLevel(),1,99,tc)
					tc:SetMaterial(mat)
					Duel.ReleaseRitualMaterial(mat)
					Duel.BreakEffect()
					Duel.SpecialSummon(tc,SUMMON_TYPE_RITUAL,tp,tp,true,false,POS_FACEUP)
					tc:CompleteProcedure()
				end
			end
end
function Auxiliary.AddPendulumProcedure(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC_G)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetRange(LOCATION_PZONE)
	e1:SetCountLimit(1,10000000)
	e1:SetCondition(Auxiliary.PendCondition())
	e1:SetOperation(Auxiliary.PendOperation())
	e1:SetValue(SUMMON_TYPE_PENDULUM)
	c:RegisterEffect(e1)
end
function Auxiliary.PConditionFilter(c,e,tp,lscale,rscale)
	local lv=c:GetLevel()
	return (c:IsLocation(LOCATION_HAND) or (c:IsFaceup() and c:IsType(TYPE_PENDULUM)))
		and lv>lscale and lv<rscale and c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_PENDULUM,tp,false,false)
		and not c:IsForbidden()
end
function Auxiliary.PendCondition()
	return	function(e,c,og)
				if c==nil then return true end
				local tp=c:GetControler()
				if c:GetSequence()~=6 then return false end
				local rpz=Duel.GetFieldCard(tp,LOCATION_SZONE,7)
				if rpz==nil then return false end
				local lscale=c:GetLeftScale()
				local rscale=rpz:GetRightScale()
				if lscale>rscale then lscale,rscale=rscale,lscale end
				local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
				if ft<=0 then return false end
				if og then
					return og:IsExists(Auxiliary.PConditionFilter,1,nil,e,tp,lscale,rscale)
				else
					return Duel.IsExistingMatchingCard(Auxiliary.PConditionFilter,tp,LOCATION_HAND+LOCATION_EXTRA,0,1,nil,e,tp,lscale,rscale)
				end
			end
end
function Auxiliary.PendOperation()
	return	function(e,tp,eg,ep,ev,re,r,rp,c,sg,og)
				local rpz=Duel.GetFieldCard(tp,LOCATION_SZONE,7)
				local lscale=c:GetLeftScale()
				local rscale=rpz:GetRightScale()
				if lscale>rscale then lscale,rscale=rscale,lscale end
				local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
				if og then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
					local g=og:FilterSelect(tp,Auxiliary.PConditionFilter,1,ft,nil,e,tp,lscale,rscale)
					sg:Merge(g)
				else
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
					local g=Duel.SelectMatchingCard(tp,Auxiliary.PConditionFilter,tp,LOCATION_HAND+LOCATION_EXTRA,0,1,ft,nil,e,tp,lscale,rscale)
					sg:Merge(g)
				end
			end
end
